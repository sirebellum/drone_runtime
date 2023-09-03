// Description: Communications with base station

// Class to handle communications
class Comm {
public:
    Comm();
    ~Comm();

    // Initialize the communications
    void init();

    // Send
    void send();

    // Receive
    void receive();
};

// WIFI communication
class WIFI: public Comm {
public:
    WIFI();
    ~WIFI();
};

// RF communication
class RF: public Comm {
public:
    RF();
    ~RF();
};