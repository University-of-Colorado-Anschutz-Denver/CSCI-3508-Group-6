// Shared by the browser (download page) and Node (scripts/fetch-release.js).
// Turns a GitHub "latest release" API response into the small object the
// website needs. One source of truth, so the page and the CI step agree.
//
// Files it looks for in a release:
//   .msi  Windows installer   (falls back to a Setup .exe until the pipeline builds an .msi)
//   .deb  Ubuntu/Debian installer, amd64
//   .pkg  macOS graphical installer      (Apple Silicon, and Intel if its name says x86_64 / intel)
//   .dmg  macOS non-graphical installer  (same rule)
(function (root, factory) {
  // Export for Node.js, or attach to the browser's global scope.
  if (typeof module === "object" && module.exports) module.exports = factory();
  else root.ReleaseData = factory();
})(typeof self !== "undefined" ? self : this, function () {
  // Convert a GitHub asset to the fields used by the website.
  function asset(a) {
    return a ? { name: a.name, url: a.browser_download_url, size: a.size || 0 } : null;
  }

  // Match file extensions without regard to letter case.
  const ext = (a, e) => a.name.toLowerCase().endsWith(e);

  // Validate the API response and select the supported release assets.
  function buildLatest(release, repo) {
    if (!release || !release.tag_name) return null;
    const assets = (release.assets || []).filter(a => a && a.name && a.browser_download_url);

    // Prefer the Windows MSI; use a setup installer or first EXE as fallback.
    const windows = assets.find(a => ext(a, ".msi"))
      || (() => {
        const exes = assets.filter(a => ext(a, ".exe"));
        return exes.find(a => /setup|install/i.test(a.name)) || exes[0];
      })()
      || null;

    // amd64 only: skip arm64 builds, and prefer a name that says amd64 / x86_64.
    const debs = assets.filter(a => ext(a, ".deb") && !/arm64|aarch64/i.test(a.name));
    const linux = debs.find(a => /amd64|x86[_-]64/i.test(a.name)) || debs[0] || null;

    // Two Mac builds. A name with x86_64 / intel / amd64 is the Intel build; anything else
    // (arm64, or no architecture in the name) is Apple Silicon.
    const isIntel = a => /x86[_-]?64|intel|amd64/i.test(a.name);
    const macPkgs = assets.filter(a => ext(a, ".pkg"));
    const macDmgs = assets.filter(a => ext(a, ".dmg"));
    const macPkg = macPkgs.find(a => !isIntel(a)) || null;
    const macDmg = macDmgs.find(a => !isIntel(a)) || null;
    const macIntelPkg = macPkgs.find(isIntel) || null;
    const macIntelDmg = macDmgs.find(isIntel) || null;
    const checksums = assets.find(a => /sha-?256/i.test(a.name)) || null;

    // Return normalized release metadata and nullable entries for each download.
    return {
      repo: repo || null,
      tag: release.tag_name,
      version: String(release.tag_name).replace(/^v/i, ""),
      publishedAt: release.published_at || null,
      htmlUrl: release.html_url || null,
      assets: {
        windows: asset(windows),
        linux: asset(linux),
        macPkg: asset(macPkg),
        macDmg: asset(macDmg),
        macIntelPkg: asset(macIntelPkg),
        macIntelDmg: asset(macIntelDmg),
        checksums: asset(checksums)
      }
    };
  }

  return { buildLatest };
});
