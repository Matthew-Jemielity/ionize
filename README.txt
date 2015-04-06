This project is a work in progress, missing makefiles and a lot of infrastrucucture.

In this project:
- plasma is a library to be used by clients,
- ionized is daemon responsible for memory management, implementing circular buffers.
- filament is a basic client-server implementation using sockets, for ionized

Clients request memory allocation from the daemon, which then passes a uid for access to that memory.
On linux shared memory will be used. Android will utilize ion.
By default ionized will use filament with ZeroMQ backend. If wanted new backend can be written, as long as it conforms to filament API.
