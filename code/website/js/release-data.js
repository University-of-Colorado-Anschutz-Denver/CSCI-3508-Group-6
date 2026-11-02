// Shared by the browser (download page) and Node (scripts/fetch-release.js).
// Turns a GitHub "latest release" API response into the small object the
// website needs. One source of truth, so the page and the CI step agree.

// UMD wrapper: exposes this module as `module.exports` under Node/CommonJS
// (e.g. scripts/fetch-release.js) and as a `ReleaseData` global in the
// browser (e.g. main.js), from the same source file.
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

    // Windows: prefer an .exe whose name suggests it's the installer
    // (e.g. "Setup.exe"/"Install.exe") over other .exe files in the release.
    const exes = assets.filter(a => /\.exe$/i.test(a.name));
    const windows = exes.find(a => /setup|install/i.test(a.name)) || exes[0] || null;

    // Linux: look for an AppImage build.
    const linux = assets.find(a => /\.appimage$/i.test(a.name)) || null;

    // Checksums file (e.g. SHA256SUMS), if the release publishes one.
    const checksums = assets.find(a => /sha-?256/i.test(a.name)) || null;

    return {
      repo: repo || null,
      tag: release.tag_name,
      version: String(release.tag_name).replace(/^v/i, ""),   // strip a leading "v" (e.g. "v1.2.0" -> "1.2.0")
      publishedAt: release.published_at || null,
      htmlUrl: release.html_url || null,
      assets: { windows: asset(windows), linux: asset(linux), checksums: asset(checksums) }
    };
  }

  // Public API of this module.
  return { buildLatest };
});
