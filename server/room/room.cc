/*
 FILNAMN: 		room.cc
 PROGRAMMERARE:	hanel742, eriek984, jened502, tobgr602, niker917, davha227
 SKAPAD DATUM:	2013-11-14
 BESKRIVNING:
 */

#include "room.h"


using namespace std;

// ----------------------------------
// Constructor

Room::Room(string inName,Master* master) {
    masterPointer = master;
    name = inName;
    readAllFromFile();
}

// ----------------------------------------
// Help functions

void ifRoomRemove(Room* inRoom) {
    User* userTemp = dynamic_cast<User*>(inRoom);
    
    if (userTemp == nullptr) {
        inRoom->masterPointer->removeRoom(inRoom->getName());
    }
}

void throwUp(Room* inRoom) {
    inRoom->chooseRoom(inRoom->parentRoom->parentRoom);
}

// ----------------------------------------
// Destructor

Room::~Room() {
    for_each(rooms.begin(), rooms.end(), ifRoomRemove);
    
    if ( parentRoom == nullptr) {
        rooms.clear();
    }
    else {
        
        parentRoom->removeRoom(this);
        
        for_each(rooms.begin(), rooms.end(), throwUp);
        parentRoom = nullptr;
    }
    
    masterPointer = nullptr;
    
}

// ----------------------------------------

unsigned int Room::getPosOfRoom(string name) const{
    for( unsigned int i = 0 ; i < rooms.size() ; i++) {
        if(rooms.at(i)->getName() == name){
            return i;
        }
    }
    throw logic_error("No such room in Rooms");
    return 0;
}

// ----------------------------------------
// Automatically called from child when child room is destroyed
void Room::removeRoom(Room* inRoom) {
    unsigned int pos = getPosOfRoom(inRoom->getName());
    rooms.at(pos) = nullptr;
    rooms.erase(rooms.begin() + pos);
}

// ----------------------------------

void Room::sendMessage(Message inMessage) {
    string to = inMessage.getTo();
    string from = inMessage.getFrom();
    
    try {
        getRoom(to)->receiveMessage(inMessage);
        getRoom(from)->receiveMessage(inMessage);
    } catch (...) {
        Message errorMessage("No such user in this room.",name,inMessage.getFrom());
    }
    
}


// ----------------------------------

void Room::listRooms(){
    for (unsigned int it = 0; it < rooms.size() ; it++){
        User* userTemp = dynamic_cast<User*>(rooms.at(it));
        if(userTemp == nullptr ){
            cout << rooms.at(it)->getName() << endl;
        }
    }
    return;
}

// ----------------------------------

void Room::listUsers(){
    for (unsigned int it = 0; it < rooms.size() ; it++){
        User* userTemp = dynamic_cast<User*>(rooms.at(it));
        if( userTemp == nullptr ){
            continue;
        }
        else{
            cout << rooms.at(it)->getName() << endl;
        }
    }
    return;
}

void Room::receiveMessage(Message inMessage) {
    string to = inMessage.getTo();
    if(to == name){
        log.push_back(inMessage);
        
        sendMessageAll(inMessage);
        saveToFile(inMessage);
    }
    else{
        sendMessage(inMessage);
    }
}

// ----------------------------------

void Room::sendMessageAll(Message inMessage) {
    
    for (unsigned int i = 0; i < rooms.size(); i++) {
        User* userTemp = dynamic_cast<User*>(rooms.at(i));
        if ( userTemp == nullptr ) {
            continue;
        }
        rooms.at(i)->receiveMessage(inMessage);
    }
}

// ----------------------------------

void Room::addRoom(Room* inRoom) {
    this->rooms.push_back(inRoom);
    inRoom->parentRoom = this;
}



// ----------------------------------

void Room::saveToFile(Message inMessage) {
    ofstream logfile;
    logfile.open ("log.txt", std::ios_base::app);
    
    logfile << inMessage.getServerTime() << endl << inMessage.getFrom() << endl;
    logfile << inMessage.getTo() << endl << inMessage.getMessage() << endl;
    
    logfile.close();
}

void Room::readAllFromFile() {
    
    string line;
    ifstream logfile ("log.txt");
    if (logfile.is_open())
    {
        for (unsigned int i=0; i<99 and getline (logfile,line); ++i) {
            vector<string> tempVector;
            
            for (unsigned int k=0; k<4; ++k) {
                getline (logfile,line);
                tempVector.push_back(line);
            }
            
            Message tempMessage(tempVector.at(3),tempVector.at(1),tempVector.at(2));
            log.push_back(tempMessage);
        }
        logfile.close();
        cout << "File was read with no errors. Read :"<<log.size()<<" Messages"<<endl;
    }
    else cout << "Unable to open file";
}



// ------------------------------------

string Room::getName() {
    return name;
}

// ------------------------------------

void Room::createRoom(std::string inName){
	addRoom(masterPointer->createRoom(inName));
}

// ------------------------------------

Room* Room::getRoom(std::string name){
    for( unsigned int i = 0 ; i < rooms.size() ; i++) {
        if(rooms.at(i)->getName() == name){
            return rooms.at(i);
        }
    }
    throw logic_error("No such room in Rooms");
    
    return nullptr;
}

// -----------------------------------

Message Room::getMessage(unsigned int i){
    if (log.size() < i){
        throw logic_error{"No message at that position!"};
    }
    return log.at(i);
}

// -----------------------------------

void Room::chooseRoom(Room*) {
    throw logic_error{"You're a retard since you're trying to move a room."};
}

Room* Room::getParentRoom() {
    return parentRoom;
}

// -----------------------------------

vector<string> User::getStruct() {
    parentRoom->getStruct();
}

// -----------------------------------

vector<string> Room::getStruct() {
    vector<string> structure;
    structure.push_back("User");
    
    for (unsigned int i = 0; i < rooms.size() ; i++) {
        User* userTemp = dynamic_cast<User*>(rooms.at(i));
        if ( userTemp == nullptr ) {
            structure.insert(structure.begin(),rooms.at(i)->getName());
        }
        else {
            structure.push_back(rooms.at(i)->getName());
        }
    }
    
    return structure;
}


