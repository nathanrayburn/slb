# Reverse Engineering Write-Up - Test Blanc

## Key Finding

- The secret is stored in the EAX register, and the assembler instruction is located at address `0x08049726`.

## Breakpoints

| Breakpoint Address | Description                                                                        |
| ------------------ | ---------------------------------------------------------------------------------- |
| 0x08049730         | Refers to the condition `if (isCharInRange != 0) {`.                               |
| 0x080493e9         | Refers to the return statement for the secret.                                     |
| 0x080493e9         | Refers to the condition `if (usernameLength == passwordLength)`.                   |
| 0x080495db         | Refers to the return statement `return otp_value_ret;` from the `getOTP` function. |
| 0x08049755         | Label `LAB_08049755`.                                                              |

## Stack Variables

- **get\_secret\_buffer**: `EBP - 0x50`
- **otp\_password**: `EBP - 0x64`
- **password**: `EBP - 0x3c`

## GDB Commands

- **Get **``** from the Stack**:

  ```
  x/s $ebp-0x64
  ```

  This command prints the string located at `EBP - 0x64`.

- **Set the Value of **``** in the Stack**:

  ```
  set {char[20]}($ebp-0x3c) = "your_new_password"
  ```

  This command sets the string value of `password` at `EBP - 0x3c` to `"your_new_password"`.

## GDB Execution Output

```
Enter your username (a-z low caps): username
Applicable mode = simple!
Enter your one-time password: password

Breakpoint 1, 0x080495db in getOTP ()
(gdb) c
Continuing.

Breakpoint 2, 0x08049755 in main ()
(gdb) x/s $ebp-0x64
0xffffcc64: "yDmchDcr"
(gdb) set {char[20]}($ebp-0x3c) = "yDmchDcr"
(gdb) c
Continuing.

Login successful

Your next passwords = YdMCHdCR, RCdHCMdY ;o)
[Inferior 1 (process 11566) exited normally]
(gdb) c
The program is not being run.
```
