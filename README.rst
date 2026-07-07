
   froot

froot is a lightweight and minimalist tool written in POSIX C to execute 
commands with elevated privileges (similar to doas or sudo).


Syscalls used:
 The core talks directly to the system using standard abstractions:

* getuid(2) / setresuid(2) -> Swapping identity and privileges.
* chroot(2) / chdir(2)    -> Directory isolation jail.
* setrlimit(2)            -> Hardware memory boundaries.
* execvp(3)               -> Executing the target command.


Fun fact: this project was designed for use on bsd, sooo it might work better on bsd-kernels XD


By: NopAngel    -   License: GPL-3.0 (`LICENSE`)
