// Unit tests for release-data.js's buildLatest(). Run with: npm test
// (or, to run just this file: node --test tests/release-data.test.js).
const test = require("node:test");
const assert = require("node:assert");
const { buildLatest } = require("../js/release-data.js");

// Builds a fake GitHub release asset object.
const asset = (name, size = 10) => ({ name, size, browser_download_url: `https://example.test/dl/${name}` });
// A full, realistic release with one asset of each kind (win64 zip, exe installer, checksums, AppImage).
const release = {
  tag_name: "v1.2.3",
  html_url: "https://github.com/o/r/releases/tag/v1.2.3",
  published_at: "2026-10-26T12:00:00Z",
  assets: [
    asset("QuadM-1.2.3-win64-portable.zip"),
    asset("QuadM-Setup-1.2.3.exe", 45000000),
    asset("SHA256SUMS.txt"),
    asset("QuadM-1.2.3.AppImage")
  ]
};

// Happy path: every asset type is present, so each should be matched to the right slot.
test("buildLatest picks the installer exe, the AppImage and the checksums", () => {
  const d = buildLatest(release, "o/r");
  assert.strictEqual(d.tag, "v1.2.3");
  assert.strictEqual(d.version, "1.2.3");
  assert.strictEqual(d.assets.windows.name, "QuadM-Setup-1.2.3.exe");
  assert.strictEqual(d.assets.windows.size, 45000000);
  assert.strictEqual(d.assets.linux.name, "QuadM-1.2.3.AppImage");
  assert.strictEqual(d.assets.checksums.name, "SHA256SUMS.txt");
});

// Tie-break: when more than one .exe is present, the one named "Setup" wins.
test("buildLatest prefers a Setup exe over another exe", () => {
  const d = buildLatest({ tag_name: "v1", assets: [asset("tool.exe"), asset("QuadM-Setup-1.exe")] });
  assert.strictEqual(d.assets.windows.name, "QuadM-Setup-1.exe");
});

// Edge cases: missing assets should yield nulls, not throw; missing/empty input should yield null overall.
test("buildLatest handles a release with no assets, and no release at all", () => {
  const d = buildLatest({ tag_name: "v0.0.1", assets: [] });
  assert.strictEqual(d.assets.windows, null);
  assert.strictEqual(d.assets.linux, null);
  assert.strictEqual(buildLatest(null), null);
  assert.strictEqual(buildLatest({}), null);
});
