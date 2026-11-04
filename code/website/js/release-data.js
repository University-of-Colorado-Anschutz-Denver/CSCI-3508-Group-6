// Shared by the browser (download page) and Node (scripts/fetch-release.js).
// Turns a GitHub "latest release" API response into the small object the
// website needs. One source of truth, so the page and the CI step agree.
//
// Files it looks for in a release:
//   .msi  Windows installer   (falls back to a Setup .exe until the pipeline builds an .msi)
//   .deb  Ubuntu/Debian installer, amd64
//   .pkg  macOS graphical installer
//   .dmg  macOS non-graphical installer
(function (root, factory) {
  if (typeof module === "object" && module.exports) module.exports = factory();
  else root.ReleaseData = factory();
})(typeof self !== "undefined" ? self : this, function () {
  // Narrow a raw GitHub release asset down to just what the site needs:
  // display name, direct download URL, and size in bytes.
  function asset(a) {
    return a ? { name: a.name, url: a.browser_download_url, size: a.size || 0 } : null;
  }

  // Normalize a GitHub "get latest release" API response into the shape
  // consumed by the download page and build scripts. Returns null if the
  // response doesn't look like a real release (e.g. repo has none yet).
  function buildLatest(release, repo) {
    if (!release || !release.tag_name) return null;

    // Only consider assets that have a name and a downloadable URL.
    const assets = (release.assets || []).filter(a => a && a.name && a.browser_download_url);

    const exes = assets.filter(a => /\.exe$/i.test(a.name));

    const windows = assets.find(a => ext(a, ".msi"))
      || (() => {
        const exes = assets.filter(a => ext(a, ".exe"));
        return exes.find(a => /setup|install/i.test(a.name)) || exes[0];
      })()
      || null;

    // amd64 only: skip arm64 builds, and prefer a name that says amd64 / x86_64.
    const debs = assets.filter(a => ext(a, ".deb") && !/arm64|aarch64/i.test(a.name));
    const linux = debs.find(a => /amd64|x86[_-]64/i.test(a.name)) || debs[0] || null;

    const macPkg = assets.find(a => ext(a, ".pkg")) || null;
    const macDmg = assets.find(a => ext(a, ".dmg")) || null;
    // Checksums file (e.g. SHA256SUMS), if the release publishes one.
    const checksums = assets.find(a => /sha-?256/i.test(a.name)) || null;

    return {
      repo: repo || null,
      tag: release.tag_name,
      version: String(release.tag_name).replace(/^v/i, ""),   // strip a leading "v" (e.g. "v1.2.0" -> "1.2.0")
      publishedAt: release.published_at || null,
      htmlUrl: release.html_url || null,
      assets: {
        windows: asset(windows),
        linux: asset(linux),
        macPkg: asset(macPkg),
        macDmg: asset(macDmg),
        checksums: asset(checksums)
      }
    };
  }

  // Public API of this module.
  return { buildLatest };
});
