# ionize
plasma is a library to be used by clients.
ionized is daemon responsible for memory management, implementing circular buffers.
Clients request memory allocation from the daemon, which then passes a uid for access to that memory.
