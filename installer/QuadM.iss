; Inno Setup script used by .github/workflows/release.yml.
; The workflow passes: AppVersion, AppExe, SourceDir (staged app folder), OutputDir.
; Harrison: replace or extend this when the real installer design is ready; the
; workflow only needs it to write QuadM-Setup-<version>.exe into OutputDir.

#ifndef AppVersion
  #define AppVersion "0.0.0-dev"
#endif
#ifndef AppExe
  #define AppExe "QuadM.exe"
#endif
#ifndef SourceDir
  #define SourceDir "..\dist"
#endif
#ifndef OutputDir
  #define OutputDir "..\release"
#endif

[Setup]
AppId={{013B5C2D-3CB1-457F-BACA-F39CEA985576}}
AppName=QuadM MultiMedia Metadata Manager
AppVersion={#AppVersion}
AppPublisher=CSCI 3508 Group 6
DefaultDirName={autopf}\QuadM
DefaultGroupName=QuadM
OutputDir={#OutputDir}
OutputBaseFilename=QuadM-Setup-{#AppVersion}
Compression=lzma2
SolidCompression=yes
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
PrivilegesRequiredOverridesAllowed=dialog
UninstallDisplayIcon={app}\{#AppExe}
WizardStyle=modern

[Files]
Source: "{#SourceDir}\*"; DestDir: "{app}"; Flags: recursesubdirs createallsubdirs ignoreversion

[Icons]
Name: "{group}\QuadM"; Filename: "{app}\{#AppExe}"
Name: "{autodesktop}\QuadM"; Filename: "{app}\{#AppExe}"; Tasks: desktopicon

[Tasks]
Name: "desktopicon"; Description: "Create a desktop shortcut"; Flags: unchecked

[Run]
Filename: "{app}\{#AppExe}"; Description: "Launch QuadM"; Flags: nowait postinstall skipifsilent
