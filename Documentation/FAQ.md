# FAQ and troubleshooting

- [I'm using PlatformIO inside VSCode and I'm getting the error *'Automaton.h: No such file or directory'*](#im-using-platformio-inside-vscode-and-im-getting-the-error-automatonh-no-such-file-or-directory)
- [I'm getting an error message after connection with Scintilla *'An OpenCore Polaris was detected, but Scintilla failed to communicate with it'*](#im-getting-an-error-message-after-connection-with-scintilla-an-opencore-polaris-was-detected-but-scintilla-failed-to-communicate-with-it)
- [The bootloader is compromized and the Anima EVO is completely un-responsive](#the-bootloader-is-compromized-and-the-anima-evo-is-completely-un-responsive)
- [I'm using VS Code with PlatformIO extension and I see lot of include errors](#im-using-vs-code-with-platformio-extension-and-i-see-lot-of-include-errors)


---

## I'm using PlatformIO inside VSCode and I'm getting the error *'Automaton.h: No such file or directory'*
>Run the following gir command from the shell:
```
git submodule update
```
---

## I'm getting an error message after connection with Scintilla *'An OpenCore Polaris was detected, but Scintilla failed to communicate with it'*
>It could be normal for older firmware. Just go ahead and upload the latest FW (*Firmware Tools > Flash latest OpenCore firmware*)

---

## The bootloader is compromized and the Anima EVO is completely un-responsive
>You need to activate the kernel bootloader and then flash the FW. The kernel bootloader is activated by connect togheter (shortcut) the two flat pads near the USB port.

---

## I'm using VS Code with PlatformIO extension and I see lot of include errors
>You can disable the *Squiggles Errors*. Go to setting and search *squiggles*. You should find *C_Cpp: Error Squiggles*. Disable it.