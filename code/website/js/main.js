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

// Open the install steps for the visitor's system and fold the other away.
function showMatchingInstallSteps() {
  const os = detectOS();
  const win = byId("steps-win"), mac = byId("steps-mac");
  if (!win || !mac) return;
  if (os === "mac") win.open = false;
  if (os === "win") mac.open = false;
}

// Populate the download page UI (version badge, OS download links, and the
// release metadata line) from the normalized release data `d`.
function showRelease(d) {
  // Version badge.
  byId("version").textContent = d.tag;
  const releasesUrl = d.htmlUrl || `https://github.com/${d.repo || REPO}/releases/latest`;

  // Windows download: link directly to the asset, or fall back to the
  // releases page if no Windows asset was published.
  const win = byId("dl-win");
  win.href = d.assets.windows ? d.assets.windows.url : releasesUrl;

  // macOS has two builds (Apple Silicon, Intel), each with a graphical installer (.pkg) and a disk image (.dmg).
  const macLinks = [
    ["dl-mac-pkg", d.assets.macPkg], ["dl-mac-dmg", d.assets.macDmg],
    ["dl-mac-intel-pkg", d.assets.macIntelPkg], ["dl-mac-intel-dmg", d.assets.macIntelDmg]
  ];
  for (const [id, asset] of macLinks) {
    const el = byId(id);
    if (!el) continue;
    if (asset) el.href = asset.url; else el.hidden = true;
  }
  // Hide a whole architecture block (and its heading) when that build is not in the release.
  for (const [blockId, a, b] of [["mac-arm", d.assets.macPkg, d.assets.macDmg], ["mac-intel", d.assets.macIntelPkg, d.assets.macIntelDmg]]) {
    const block = byId(blockId);
    if (block && !a && !b) block.hidden = true;
  }
  const macCard = byId("mac-arm") && byId("mac-arm").closest(".card");
  if (macCard && !macLinks.some(([, asset]) => asset)) macCard.hidden = true;   // no Mac build in this release

  // Highlight the download that matches the visitor's computer.
  const mine = { mac: "dl-mac-pkg", win: "dl-win", linux: "dl-linux" }[detectOS()];
  const mineEl = mine && byId(mine);
  const mineCard = mineEl && mineEl.closest(".card");
  if (mineCard && !mineCard.hidden) mineCard.classList.add("recommended");

  // Linux download: link to the asset if present; the row stays hidden otherwise
  // rather than show a broken/irrelevant link.
  const linux = byId("dl-linux");
  const linuxRow = byId("linux-row");
  if (linux && d.assets.linux) { linux.href = d.assets.linux.url; if (linuxRow) linuxRow.hidden = false; }

  // Optional metadata line: publish date, Windows asset size, and whether
  // checksums are available. Only the pieces that exist are shown.
  const meta = byId("release-meta");
  if (meta) {
    const bits = [];
    if (d.publishedAt) bits.push("Released " + new Date(d.publishedAt).toLocaleDateString(undefined, { year: "numeric", month: "short", day: "numeric" }));
    if (d.assets.windows && d.assets.windows.size) bits.push((d.assets.windows.size / 1048576).toFixed(0) + " MB");
    if (d.assets.checksums) bits.push("checksums available on the release page");
    meta.textContent = bits.join(" · ");
  }
}

// Only run the release lookup on pages that actually have a #version element.
// On failure (e.g. network issue, no releases yet), say so and keep the buttons
// pointing at the GitHub releases page instead of leaving the badge blank.
if (byId("version")) {
  showMatchingInstallSteps();
  loadLatest().then(showRelease).catch(() => {
    byId("version").textContent = "see GitHub Releases";
    const meta = byId("release-meta");
    if (meta) meta.textContent = "We couldn't check for the latest version. The buttons below open the GitHub Releases page.";
  });
}


// ---------------------------------------------------------------------------
// Feedback form
// Validates each field while the visitor types (using the same rules as the
// server-side check in js/feedback-service.js), shows clear messages, and
// stores the message in Cloud Firestore through FeedbackService. Runs only if
// the form exists on the current page.
// ---------------------------------------------------------------------------
const form = byId("feedback-form");
if (form) initFeedbackForm(form);

function initFeedbackForm(form) {
  const status = byId("form-status");
  const submitBtn = form.querySelector("button[type=submit]");
  const counter = byId("message-count");
  const FIELDS = ["type", "email", "message", "version"];
  const MAX = 2000;
  const touched = new Set();      // fields the visitor has already left once
  let sent = false;               // true right after a successful send

  const values = () => Object.fromEntries(new FormData(form));

  // Show or clear the inline error under one field.
  function showFieldError(name, message) {
    const input = form.elements[name], err = byId("err-" + name);
    if (message) { err.textContent = message; err.hidden = false; input.setAttribute("aria-invalid", "true"); }
    else { err.textContent = ""; err.hidden = true; input.removeAttribute("aria-invalid"); }
  }
  function validateField(name) {
    const message = FeedbackService.validate(values()).errors[name];
    showFieldError(name, message);
    return !message;
  }
  function updateCounter() {
    const n = form.elements.message.value.length;
    counter.textContent = `${n} / ${MAX}`;
    counter.classList.toggle("warn", n > MAX * 0.9);
  }
  function setBusy(busy) {
    submitBtn.disabled = busy;
    submitBtn.classList.toggle("loading", busy);
    submitBtn.textContent = busy ? "Sending…" : "Send feedback";
    form.setAttribute("aria-busy", String(busy));
  }
  const hasUnsentText = () => !sent && form.elements.message.value.trim().length > 0;

  // Real-time validation: check a field when the visitor leaves it, then keep
  // re-checking as they edit it (so the error disappears as soon as it is fixed).
  for (const name of FIELDS) {
    const el = form.elements[name];
    el.addEventListener("blur", () => { touched.add(name); validateField(name); });
    el.addEventListener("input", () => {
      sent = false;
      if (name === "message") updateCounter();
      if (touched.has(name)) validateField(name);
    });
  }
  updateCounter();

  // Expert shortcut: Ctrl+Enter (or Cmd+Enter) in the message box sends the form.
  form.elements.message.addEventListener("keydown", e => {
    if ((e.ctrlKey || e.metaKey) && e.key === "Enter") { e.preventDefault(); form.requestSubmit(); }
  });

  // Warn before leaving the page with a message that was not sent.
  window.addEventListener("beforeunload", e => {
    if (hasUnsentText()) { e.preventDefault(); e.returnValue = ""; }
  });

  form.addEventListener("submit", async e => {
    e.preventDefault();                              // handle the submit ourselves via fetch
    const data = values();
    if (data.website) return;                        // honeypot: bots fill this
    clearStatus(status);

    // Check every field first and point at the first problem.
    const errors = FeedbackService.validate(data).errors;
    FIELDS.forEach(name => { touched.add(name); showFieldError(name, errors[name]); });
    const firstBad = FIELDS.find(name => errors[name]);
    if (firstBad) {
      setStatus(status, "err", "Please fix the highlighted field" + (Object.keys(errors).length > 1 ? "s" : "") + " and try again.");
      form.elements[firstBad].focus();
      return;
    }

    setBusy(true);
    setStatus(status, "info", "Sending your feedback…");
    const res = await FeedbackService.submit(data, FIREBASE);
    setBusy(false);

    if (res.ok) {
      form.reset(); touched.clear(); sent = true; updateCounter();
      FIELDS.forEach(name => showFieldError(name, ""));
      setStatus(status, "ok", "Thanks! Your feedback was sent.");
    } else if (res.reason === "invalid") {
      setStatus(status, "err", Object.values(res.errors)[0]);
    } else if (res.reason === "not-configured") {
      setStatus(status, "warn", "Feedback isn't available right now. Please try again later, or contact a team member from the About page.");
    } else if (res.reason === "network") {
      setStatus(status, "err", "We couldn't reach the server. Check your internet connection and try again. Your message is still here.");
    } else {
      setStatus(status, "err", "Something went wrong on our side (" + res.reason + "). Your message is still here, so you can try again in a moment.");
    }
  });
}
