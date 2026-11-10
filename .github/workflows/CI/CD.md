# QuadM CI/CD pipeline

Two workflows. Nothing else to run by hand.

| You do | What happens |
| --- | --- |
| Open a pull request that touches `code/website/` | `website.yml`: lint, tests, builds the Docker image and smoke-tests it. Nothing is published. |
| Push to a branch listed in `website.yml` (`main`, `testing`, `website_skeleton`) | Same checks, then the image is pushed to `ghcr.io/<owner>/quadm-website` (tags: branch name, short commit hash, and `latest` on `main`). |
| `git tag v0.4.0 && git push origin v0.4.0` | `release.yml` builds the installers with the scripts in `code/packaging/`, in parallel: Windows `QuadM-0.4.0-windows-x64.msi` (`build-windows-msi.ps1`), Ubuntu/Debian `quadm_0.4.0_amd64.deb` (`build-deb.sh`), macOS `QuadM-0.4.0-macos-arm64.pkg` and `.dmg` on an Apple Silicon runner plus `QuadM-0.4.0-macos-x86_64.pkg` and `.dmg` on an Intel runner (`build-macos.sh`, the `macos-intel` job). Each is installed on a clean runner and checked. Then it publishes a GitHub Release with the files and `SHA256SUMS.txt`. |
| (automatic, after the release is published) | `release.yml` starts `website.yml`, which writes `data/latest.json` from the new release and publishes a new website image. The download page shows the new version and links. |

The Windows `.msi` decides whether the release goes out. If only the Linux or macOS job fails, the release is still published without that file, and the publish job shows a warning.

The download page reads `data/latest.json` first (baked into the image) and asks the GitHub API only if that file is missing, so a plain checkout of the site still works.

## One-time setup

1. **Settings -> Actions -> General -> Workflow permissions:** *Read and write permissions*. The workflows ask for their own permissions, but an organization can lock these down.
2. **`code/website/js/main.js`:** set `REPO` to `owner/repo`, and `FIREBASE` for the feedback form (`firebase/README.md`).
3. **`release.yml`, the `env:` block:** `QT_VERSION` is the Qt used on the runners; `EXIFTOOL_VERSION` is the ExifTool version for the `.deb` and macOS builds. The Windows build uses the zip committed in `code/packaging/windows-deps/`. The packaging scripts on a laptop may use another Qt; that is fine.
4. **After the first image push:** GitHub -> Packages -> `quadm-website` -> Package settings -> visibility Public, so anyone can pull it.
5. **Branch protection (optional):** require the check **Lint and test website** on `main`.
6. The tag filter is `v<number>.<number>.<number>`. The installers need a plain numeric version, so `v0.4.0` works and `Beta` or `v0.4.0-rc1` does not.

7. **ExifTool for Windows (once, and whenever you want a newer one):** download `exiftool-XX.XX_64.zip` (Windows Executable) from https://exiftool.org in a browser and commit it, unzipped-as-is, to `code/packaging/windows-deps/` (`git add code/packaging/windows-deps/exiftool-13.60_64.zip`). The release workflow uses the newest `exiftool-*_64.zip` in that folder. Alternative without a commit: a pre-release tagged `tools-exiftool` with the zip attached (untick *Set as the latest release*).

## Run the website image

```
docker run --rm -p 8080:8080 ghcr.io/<owner>/quadm-website:latest
```
Open http://localhost:8080. The container listens on 8080 and runs as a non-root user.

## Check locally before pushing

```
cd code/website
npm run check          # HTML lint + all tests
```

To build an installer yourself, follow `code/Packaging.md` (Windows, Linux) and `code/Packaging-macOS.md`.

## If a run fails

| Symptom | Likely cause |
| --- | --- |
| Windows job: `MinGW not found at ...` | `install-qt-action` put the compiler somewhere else; check the log of the *Install Qt* step and adjust the path in *Build the MSI*. |
| Windows job: `No ExifTool zip` | The zip is missing from `code/packaging/windows-deps/` (see "One-time setup" step 7) and there is no `tools-exiftool` release. The public hosts block GitHub's runners (SourceForge answers 403). |
| Windows job: `wix` errors | The script pins WiX 5.0.2; the log of the *Build the MSI* step shows the first error. |
| Windows job: QuadM exits right after start | A DLL is missing from the stage folder; the install log is in the step output. |
| Linux job: `ui_mainwindow.h` not found | `src/QuadMGUI/MainWindow.ui` still has a capital M (step 9 renames it). |
| Linux job: `timeout` code is not 124 | The app crashed on start in the offscreen platform; run the step's commands in WSL to see the message. |
| macOS job: `Qt not found` | `brew install qt` failed or `brew --prefix qt` is empty; read the install step. |
| macOS job: `exiftool -ver` fails | Perl or the ExifTool download is broken; `fetch-exiftool-unix.sh` stops there on purpose. |
| Release exists but a file is missing | That job failed; fix it and re-run the failed jobs (the publish job runs again with them). Re-running `publish` is safe: files of an existing release are replaced. |
| Download button missing on the website | The page looks for `.msi` (or a Setup `.exe`), an amd64 `.deb`, `.pkg` and `.dmg` in the latest release; a card hides itself when its file is not there. |
| Website still shows the old version after a release | Open Actions -> *Website checks* and check the run named `release vX.Y.Z`; re-run it. |
| A manually created release (for example the tag `Beta`) is shown as latest | The page shows GitHub's "latest" release. Mark the newest tagged release as latest, or delete the manual one. |
| Image push is denied | Workflow permissions are read-only (step 1), or the package exists under another owner. |

## What is not signed or tested yet

The installers are not code-signed, so Windows SmartScreen warns. The macOS app is ad-hoc signed but not notarized (that needs a paid Apple Developer account): on first launch users right-click the app and choose *Open*. The macOS build is Apple Silicon only, because `build-macos.sh` builds for the machine it runs on; for an Intel build add a second job on `macos-15-intel`.

The three installer jobs have never run on GitHub: the first tag push is the real test, so try `v0.0.1` before the demo.
