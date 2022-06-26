# Prokcy

Simple proxy in C to try some network hacking.

Only supports IPv4 and no name resolution because I'm too lazy to do it now.

## Compile & Run

```sh
mkdir build && cmake -B build/ && cmake --build build/

./build/prokcy 127.0.0.1 21 192.0.2.11 21
# Arguments are: [<bind_address>] <bind_port> <target_address> <target_port>
```
