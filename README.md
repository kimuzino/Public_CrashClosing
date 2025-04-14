# Public_CrashClosing
Example what should be inside the config.yaml file that is located where the CrashClosing.exe is

## Example of the .yaml file
```bash
Applications:
  - destiny2.exe
  - CS2.exe
EnableTimer: false
Timer: 30
EnableCloseButton: true
CloseButton: 0x2E
SystemTray: true
CloseOnly: false
```
To map your own key to CloseButton: you have to go to https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes website and find you desired keys value.