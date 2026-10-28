// ---------------------------------------------------------------------------
// Configuration
// Set these once the repo and feedback backend exist.
// ---------------------------------------------------------------------------
const REPO = "YOUR-USERNAME/YOUR-REPO";            // owner/name on GitHub
const FEEDBACK_ENDPOINT = "";                       // e.g. a Cloud Function / Formspree URL

// ---------------------------------------------------------------------------
// Download page
// Fetches the latest GitHub release and renders its version/assets/metadata
// into the page. Runs only when a #version element exists (i.e. on the
// download page), so this script is safe to include site-wide.
// ---------------------------------------------------------------------------

// Ask GitHub's REST API for the latest release, then normalize it via
// ReleaseData.buildLatest (defined elsewhere) into the shape showRelease expects.
async function loadLatest() {
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

  // Linux download: link to the asset if present, otherwise hide the card
  // entirely rather than show a broken/irrelevant link.
  const linux = document.getElementById("dl-linux");
  if (d.assets.linux) linux.href = d.assets.linux.url;
  else if (linux.closest(".card")) linux.closest(".card").hidden = true;   // no Linux build in this release

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
// posts the data to FEEDBACK_ENDPOINT as JSON. Runs only if the form exists
// on the current page.
// ---------------------------------------------------------------------------
const form = document.getElementById("feedback-form");
if (form) {
  form.addEventListener("submit", async e => {
    e.preventDefault();                              // handle the submit ourselves via fetch
    const status = document.getElementById("form-status");
    const data = Object.fromEntries(new FormData(form));
    if (data.website) return;                        // honeypot: bots fill this
    if (!data.message || !data.message.trim()) { status.textContent = "Please write a message."; return; }
    if (!FEEDBACK_ENDPOINT) { status.textContent = "Feedback backend not connected yet."; return; }
    try {
      const r = await fetch(FEEDBACK_ENDPOINT, { method: "POST", headers: { "Content-Type": "application/json" }, body: JSON.stringify(data) });
      if (!r.ok) throw new Error();
      form.reset(); status.textContent = "Thanks! Your feedback was sent.";
    } catch { status.textContent = "Could not send. Please try again later."; }
  });
}
