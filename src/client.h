#ifndef CLIENT_HEADER
#define CLIENT_HEADER
#include <map>
#include "connection.h"
#include "environment.h"
#include "common_irrlicht.h"
#include <jmutex.h>
using namespace jthread;

// -----for storing node with c++ map-----
struct NodePos {
public:
	NodePos(int x, int y, int z) :
			x(x), y(y), z(z) {
	}
	int x;
	int y;
	int z;

	bool operator<(const NodePos& t) const {
		if ((this->x == t.x) && (this->y == t.y))
			return (this->z < t.z);
		else if (this->x == t.x)
			return (this->y < t.y);
		else
			return (this->x < t.x);
	}

	bool operator!=(const NodePos& t) const {
		if ((this->x == t.x) && (this->y == t.y) && (this->z == t.z))
			return false;
		else
			return true;
	}
	bool operator =(const NodePos& t) const {
		if ((this->x == t.x) && (this->y == t.y) && (this->z == t.z))
			return true;
		else
			return false;
	}
};

class Client;

class ClientUpdateThread: public JThread {
	bool run;
	JMutex run_mutex;

	Client *m_client;

public:

	ClientUpdateThread(Client *client) :
			JThread(), run(true), m_client(client) {
		run_mutex.Init();
	}

	void * Thread();

	bool getRun() {
		run_mutex.Lock();
		bool run_cached = run;
		run_mutex.Unlock();
		return run_cached;
	}
	void setRun(bool a_run) {
		run_mutex.Lock();
		run = a_run;
		run_mutex.Unlock();
	}
};

struct IncomingPacket {
	IncomingPacket() {
		m_data = NULL;
		m_datalen = 0;
		m_refcount = NULL;
	}
	IncomingPacket(const IncomingPacket &a) {
		m_data = a.m_data;
		m_datalen = a.m_datalen;
		m_refcount = a.m_refcount;
		if (m_refcount != NULL)
			(*m_refcount)++;
	}
	IncomingPacket(u8 *data, u32 datalen) {
		m_data = new u8[datalen];
		memcpy(m_data, data, datalen);
		m_datalen = datalen;
		m_refcount = new s32(1);
	}
	~IncomingPacket() {
		if (m_refcount != NULL) {
			assert(*m_refcount > 0);
			(*m_refcount)--;
			if (*m_refcount == 0) {
				if (m_data != NULL)
					delete[] m_data;
			}
		}
	}
	/*IncomingPacket & operator=(IncomingPacket a)
	 {
	 m_data = a.m_data;
	 m_datalen = a.m_datalen;
	 m_refcount = a.m_refcount;
	 (*m_refcount)++;
	 return *this;
	 }*/
	u8 *m_data;
	u32 m_datalen;
	s32 *m_refcount;
};

class Client {
public:
	/*
	 NOTE: Every public method should be thread-safe
	 */
	Client(scene::ISceneManager* smgr, video::SMaterial *materials);
	~Client();
	void connect(Address address);
	/*
	 Stuff that references the environment is valid only as
	 long as this is called. (eg. Players)
	 */
	void step(float dtime);

	void ProcessData(u8 *data, u32 datasize, u16 peer_id);
	// Returns true if something was done
	bool AsyncProcessData();
	void Send(u16 channelnum, SharedBuffer<u8> data, bool reliable);

	// Initiates block transfer
	void fetchBlock(v3s16 p);

	// Pops out a packet from the packet queue
	IncomingPacket getPacket();

	void removeNode(v3s16 nodepos);
	void addNode(v3s16 nodepos, MapNode n);

	void updateCamera(v3f pos, v3f dir);

	MapNode getNode(v3s16 p);

	// Return value is valid until client is destroyed
	Player * getLocalPlayer();
	// Return value is valid until step()
	core::list<Player*> getPlayers();
	core::list<Npc*> getNpcs();

	// -----for saving map-----
	void saveMap();
private:
	// -----for saving map-----
	int getNodeType(u8 node);

	// -----get MapNode position range of the given MapBlock position
	int minVal(int v) {
		return v * MAP_BLOCKSIZE;
	}

	void ReceiveAll();
	void Receive();

	// m_con_mutex must be locked when calling these
	void sendPlayerPos(float dtime);

	ClientUpdateThread m_thread;

	Environment m_env;
	JMutex m_env_mutex;

	con::Connection m_con;
	JMutex m_con_mutex;

	// -----for saving map, omit air node-----
	std::map<NodePos, int> m_nodes;

	core::map<v3s16, float> m_fetchblock_history;
	//core::list<v3s16> m_fetchblock_queue;
	JMutex m_fetchblock_mutex;

	core::list<IncomingPacket> m_incoming_queue;
	JMutex m_incoming_queue_mutex;

	scene::ISceneManager* m_smgr;
};

#endif

