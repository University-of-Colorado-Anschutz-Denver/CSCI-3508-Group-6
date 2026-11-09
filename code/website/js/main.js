// ---------------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------------
const REPO = "University-of-Colorado-Anschutz-Denver/CSCI-3508-Group-6";   // owner/name on GitHub
const FIREBASE = { projectId: "quadmdatabase", apiKey: "AIzaSyBEJ8hqkAfUM4wEWumKLhwnccG6hhm0-Q4" };

// ---------------------------------------------------------------------------
// Shared helpers
// ---------------------------------------------------------------------------
const byId = id => document.getElementById(id);

// Which operating system is the visitor on? Returns "win", "mac", "linux" or null.
// Used to recommend the right download and open the matching install steps.
function detectOS() {
  const plat = (navigator.userAgentData && navigator.userAgentData.platform) || navigator.platform || "";
  return /mac/i.test(plat) ? "mac" : /win/i.test(plat) ? "win" : /linux/i.test(plat) ? "linux" : null;
}

// Small inline icons for status messages (static markup, same style as the pages).
const STATUS_ICONS = {
  ok:   '<circle cx="12" cy="12" r="10"/><path d="M8 12l3 3 5-6"/>',
  err:  '<circle cx="12" cy="12" r="10"/><path d="M9 9l6 6M15 9l-6 6"/>',
  warn: '<path d="M12 3l10 18H2z"/><path d="M12 10v5M12 18h.01"/>',
  info: '<circle cx="12" cy="12" r="10"/><path d="M12 11v6M12 7.5h.01"/>'
};

// Show a message in a status box: colored by meaning (ok/err/warn/info) and
// always with an icon and text, so color is never the only signal.
function setStatus(el, kind, text) {
  el.className = "status " + kind;
  el.innerHTML = `<svg class="icon" aria-hidden="true" viewBox="0 0 24 24">${STATUS_ICONS[kind]}</svg>`;
  const span = document.createElement("span");
  span.textContent = text;                          // text only: never inject markup
  el.appendChild(span);
}
function clearStatus(el) { el.className = "status"; el.textContent = ""; }

// ---------------------------------------------------------------------------
// Home page
// Make the main button match the visitor's computer ("Download for macOS").
// ---------------------------------------------------------------------------
(function personalizeHomeButton() {
  const cta = byId("cta-download");
  if (!cta) return;
  const label = { win: "Download for Windows", mac: "Download for macOS", linux: "Download for Linux" }[detectOS()];
  if (label) cta.textContent = label;
})();

// ---------------------------------------------------------------------------
// Download page
// Fetches the latest release and renders its version/assets/metadata into the
// page. Runs only when a #version element exists (i.e. on the download page),
// so this script is safe to include site-wide.
// It prefers data/latest.json (baked into the Docker image by CI on every
// release) and falls back to asking GitHub's REST API directly; either way the
// result has the shape ReleaseData.buildLatest returns, which showRelease expects.
// ---------------------------------------------------------------------------
async function loadLatest() {
  try {
    const r = await fetch("data/latest.json", { cache: "no-cache" });
    if (r.ok) {
      const d = await r.json();
      if (d && d.tag) return d;
    }
  } catch (e) { /* not baked in (plain checkout): use the API below */ }
  const r = await fetch(`https://api.github.com/repos/${REPO}/releases/latest`, {
    headers: { Accept: "application/vnd.github+json" }
  });
  if (!r.ok) throw new Error("release lookup failed");
  return ReleaseData.buildLatest(await r.json(), REPO);
}

// Populate the download page UI (version badge, OS download links, and the
// release metadata line) from the normalized release data `d`.
function showRelease(d) {
  // Version badge.
  const verEl = document.getElementById("version");
  verEl.textContent = d.tag;
  const releasesUrl = d.htmlUrl || `https://github.com/${d.repo || REPO}/releases/latest`;

  // Windows download: link directly to the asset, or fall back to the
  // releases page if no Windows asset was published.
  const win = document.getElementById("dl-win");
  win.href = d.assets.windows ? d.assets.windows.url : releasesUrl;

  // macOS has two files: a graphical installer (.pkg) and a disk image (.dmg).
  const macPkg = document.getElementById("dl-mac-pkg");
  const macDmg = document.getElementById("dl-mac-dmg");
  if (macPkg) { if (d.assets.macPkg) macPkg.href = d.assets.macPkg.url; else macPkg.hidden = true; }
  if (macDmg) { if (d.assets.macDmg) macDmg.href = d.assets.macDmg.url; else macDmg.hidden = true; }
  const macCard = (macPkg || macDmg) && (macPkg || macDmg).closest(".card");
  if (macCard && !d.assets.macPkg && !d.assets.macDmg) macCard.hidden = true;   // no Mac build in this release

  // Highlight the download that matches the visitor's computer.
  const plat = (navigator.userAgentData && navigator.userAgentData.platform) || navigator.platform || "";
  const mine = /mac/i.test(plat) ? "dl-mac-pkg" : /win/i.test(plat) ? "dl-win" : /linux/i.test(plat) ? "dl-linux" : null;
  const mineEl = mine && document.getElementById(mine);
  if (mineEl && mineEl.closest(".card") && !mineEl.closest(".card").hidden) mineEl.closest(".card").classList.add("recommended");

  // Linux download: link to the asset if present; the row stays hidden otherwise
  // rather than show a broken/irrelevant link.
  const linux = document.getElementById("dl-linux");
  const linuxRow = document.getElementById("linux-row");
  if (linux && d.assets.linux) { linux.href = d.assets.linux.url; if (linuxRow) linuxRow.hidden = false; }

  // Optional metadata line: publish date, Windows asset size, and whether
  // checksums are available. Only the pieces that exist are shown.
  const meta = document.getElementById("release-meta");
  if (meta) {
    const bits = [];
    if (d.publishedAt) bits.push("Released " + new Date(d.publishedAt).toLocaleDateString(undefined, { year: "numeric", month: "short", day: "numeric" }));
    if (d.assets.windows && d.assets.windows.size) bits.push((d.assets.windows.size / 1048576).toFixed(0) + " MB");
    if (d.assets.checksums) bits.push("checksums available on the release page");
    meta.textContent = bits.join(" · ");
  }
}

// Only run the release lookup on pages that actually have a #version element.
// On failure (e.g. network issue, no releases yet), fall back to a generic
// message instead of leaving the badge blank.
if (document.getElementById("version")) {
  loadLatest().then(showRelease).catch(() => {
    document.getElementById("version").textContent = "see GitHub Releases";
  });
}

// ---------------------------------------------------------------------------
// Feedback form
// Intercepts the form submit, does basic validation/spam-filtering, and
// stores the message in Cloud Firestore through FeedbackService
// (js/feedback-service.js). Runs only if the form exists on the current page.
// ---------------------------------------------------------------------------
const form = document.getElementById("feedback-form");
if (form) {
  form.addEventListener("submit", async e => {
    e.preventDefault();                              // handle the submit ourselves via fetch
    const status = document.getElementById("form-status");
    const data = Object.fromEntries(new FormData(form));
    if (data.website) return;                        // honeypot: bots fill this
    const btn = form.querySelector("button[type=submit]");
    btn.disabled = true; status.textContent = "Sending…";
    const res = await FeedbackService.submit(data, FIREBASE);
    btn.disabled = false;
    status.className = res.ok ? "ok" : "err";        // colors the message (see style.css)
    if (res.ok) { form.reset(); status.textContent = "Thanks! Your feedback was sent."; }
    else if (res.reason === "invalid") status.textContent = Object.values(res.errors)[0];
    else if (res.reason === "not-configured") status.textContent = "Feedback backend not connected yet.";
    else status.textContent = "Could not send. Please try again later.";
  });
}
