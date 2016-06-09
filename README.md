chenor
===
>granter of wishes

A write only filesystem using public key cryptography.

This was a project to create a *write* only filesystem using C++, Botan and Fuse; the purpose being that once data is written to disk and flushed from volatile memory, it is no longer possible to retrieve that data, even if the machine is seized or compromised *after* the fact.

The encrypted data can then be transferred to a more trustworthy machine which has the private key and can decrypt the data and read it back.

This may be useful for an outdoor sensor for example, the machine is trusted initially but could be stolen later in which case there would be no way for the attacker to decrypt any data held on disk.

This differs from symmetric encryption because if a device using symmetric encryption is captured while running, the attacker can retrieve the encryption key from memory and decrypt all data. Additionally such a device requires manual setup and cannot recover after a power failure (unless the encryption key is stored on permanent storage in plaintext in which case you might as well not be using encryption).

It was a fun project but unfortunately my use case disappeared so I stopped working on it. The encryption side is finished, and there are numerous unit tests. If you are interested in using the Botan library, these tests may serve as helpful examples.
