const test = require("node:test");
const assert = require("node:assert");
const { buildLatest } = require("../js/release-data.js");

const asset = (name, size = 10) => ({ name, size, browser_download_url: `https://example.test/dl/${name}` });
const release = {
  tag_name: "v1.2.3",
  html_url: "https://github.com/o/r/releases/tag/v1.2.3",
  published_at: "2026-10-26T12:00:00Z",
  assets: [
    asset("QuadM-1.2.3-win64-portable.zip"),
    asset("QuadM-1.2.3.msi", 45000000),
    asset("quadm_1.2.3_amd64.deb"),
    asset("QuadM-1.2.3.pkg", 60000000),
    asset("QuadM-1.2.3.dmg", 58000000),
    asset("SHA256SUMS.txt")
  ]
};

test("buildLatest finds the .msi, .deb, .pkg, .dmg and the checksums", () => {
  const d = buildLatest(release, "o/r");
  assert.strictEqual(d.tag, "v1.2.3");
  assert.strictEqual(d.version, "1.2.3");
  assert.strictEqual(d.assets.windows.name, "QuadM-1.2.3.msi");
  assert.strictEqual(d.assets.windows.size, 45000000);
  assert.strictEqual(d.assets.linux.name, "quadm_1.2.3_amd64.deb");
  assert.strictEqual(d.assets.macPkg.name, "QuadM-1.2.3.pkg");
  assert.strictEqual(d.assets.macDmg.name, "QuadM-1.2.3.dmg");
  assert.strictEqual(d.assets.checksums.name, "SHA256SUMS.txt");
});

test("extensions are matched case-insensitively", () => {
  const d = buildLatest({ tag_name: "v1", assets: [asset("QuadM.MSI"), asset("QUADM.DEB"), asset("QuadM.PKG"), asset("QuadM.DMG")] });
  assert.ok(d.assets.windows && d.assets.linux && d.assets.macPkg && d.assets.macDmg);
});

test("Windows prefers the .msi over an .exe", () => {
  const d = buildLatest({ tag_name: "v1", assets: [asset("QuadM-Setup-1.exe"), asset("QuadM-1.msi")] });
  assert.strictEqual(d.assets.windows.name, "QuadM-1.msi");
});

test("Windows falls back to a Setup .exe while no .msi is published", () => {
  const d = buildLatest({ tag_name: "v1", assets: [asset("tool.exe"), asset("QuadM-Setup-1.exe")] });
  assert.strictEqual(d.assets.windows.name, "QuadM-Setup-1.exe");
});

test("Linux takes the amd64 .deb and never an arm64 one", () => {
  const both = buildLatest({ tag_name: "v1", assets: [asset("quadm_1_arm64.deb"), asset("quadm_1_amd64.deb")] });
  assert.strictEqual(both.assets.linux.name, "quadm_1_amd64.deb");
  const armOnly = buildLatest({ tag_name: "v1", assets: [asset("quadm_1_arm64.deb"), asset("quadm_1_aarch64.deb")] });
  assert.strictEqual(armOnly.assets.linux, null);
  const plain = buildLatest({ tag_name: "v1", assets: [asset("quadm.deb")] });
  assert.strictEqual(plain.assets.linux.name, "quadm.deb");
});

test("old file types (.AppImage, .zip) are not mistaken for installers", () => {
  const d = buildLatest({ tag_name: "v1", assets: [asset("QuadM-1.AppImage"), asset("QuadM-portable.zip")] });
  assert.strictEqual(d.assets.windows, null);
  assert.strictEqual(d.assets.linux, null);
  assert.strictEqual(d.assets.macPkg, null);
  assert.strictEqual(d.assets.macDmg, null);
});

test("each file type can be missing on its own", () => {
  const d = buildLatest({ tag_name: "v1", assets: [asset("QuadM-1.pkg")] });
  assert.strictEqual(d.assets.macPkg.name, "QuadM-1.pkg");
  assert.strictEqual(d.assets.macDmg, null);
  assert.strictEqual(d.assets.windows, null);
});

test("buildLatest handles a release with no assets, and no release at all", () => {
  const d = buildLatest({ tag_name: "v0.0.1", assets: [] });
  for (const k of ["windows", "linux", "macPkg", "macDmg", "checksums"]) assert.strictEqual(d.assets[k], null);
  assert.strictEqual(buildLatest(null), null);
  assert.strictEqual(buildLatest({}), null);
});
