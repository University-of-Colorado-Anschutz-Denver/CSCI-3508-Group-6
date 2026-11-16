# Website skeleton

Plain HTML/CSS/JS, no build step.

| Page | Purpose |
|---|---|
| index.html | Landing: what it is, features, formats, screenshot |
| download.html | Latest release (pulled from GitHub Releases API) |
| guide.html | User guide |
| feedback.html | Feedback form |
| about.html | Team and links |

## TODO
- Replace `YOUR-USERNAME/YOUR-REPO` in the HTML and in `js/main.js`.
- Add screenshots to `assets/` (the GUI mockup PNG works as a placeholder).
- Connect feedback storage: follow `firebase/README.md` and fill `FIREBASE` in `js/main.js`.
- Deploy: GitHub Pages from `code/website` (Actions workflow or a `gh-pages` branch).

Preview locally: `python3 -m http.server 8000` in this folder.
