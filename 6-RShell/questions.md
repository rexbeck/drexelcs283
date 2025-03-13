1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

_The remote client determines it through the receival of EOF character. The recv command is able to handle partial reads as it continually updates the buffer until the specific EOF character is received and the loop is broken._

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

_You need to set boundaries for the retrieval of the data stream. This can be done through setting an upper limit to the amount of data accepted and by only ending the retrieval if the limit is reached or a specific EOF character is received as the last part of the stream. If not handled correctly, you can run into issues with processing the data streamed through. You might have issues of invisible characters, missing parts of the stream, or memory allocation issues._

3. Describe the general differences between stateful and stateless protocols.

_Stateful protocols are what we did for this assignment. It keeps track of all sessions during it's lifetime, and requests can connect with previous ones. A Stateless protocol does not keep track of sessions at all and each one is independent from the one before._

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

_It offers benefites like lower latency, speed, and simplicity. It's best for applications where lower latency is more critical._

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

_The socket interface is provided by the operating system. Additionally, functions like bind(), listen(), accept(), etc allows easier network communication._