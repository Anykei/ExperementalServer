#pragma once

#include "NetworkClient.h"
#include <vector>
#include <mutex>

class TaskList
{
public:
	TaskList() {}
	~TaskList() {}

	void add_client(Client *c);
	void return_clients(std::vector<Client*> *client_list);
	Task_Client * take_firs();
	void addTask(Task_Client *task);
	int get_count();

	bool getWorkThread() { return work_thread; }

	void lock_thread();
	void unlock_thread();
	void thread_end();

private:
	std::mutex client_list_lock;
	std::mutex lock_task_list;
	std::mutex wait_task;
	std::mutex count_lock;
	std::condition_variable cv;


	

	bool work_thread = true;
	int task_count{ 0 };
	bool empty = true;
	std::vector<Task_Client*> taskList;
	std::vector<Client*> return_client_list;
};

void sendThread(TaskList *task_list);
void recvThread(TaskList *task_list);

struct Thread_task
{
	std::thread servise_thread;
	TaskList all_task;

	void initSendThread()
	{
		servise_thread = std::thread(sendThread, &all_task);
	}
	void initRecvThread()
	{
		servise_thread = std::thread(recvThread, &all_task);
	}
};

class NetworkTaskManager
{
public:
	NetworkTaskManager();
	~NetworkTaskManager();
	
	void addTask(Task_Client *t);
	void addTask(CString *str, Client *client);
	void return_client_list(std::vector<Client*> *list);
	
	void start();
	void start_task_manager();
private:
	
	void return_client();
	void create_thred();
	void in_wait_task();
	void start_task();
	int find_optimal_thread(int *count_task, int size);

	std::mutex modifyTaskList;
	std::mutex control_client_list;
	std::mutex control_task_list;
	std::mutex wait_task;
	std::condition_variable cv;

	bool work_server;
	bool is_task = false;

	std::mutex lock_task_list;

	Task_Client *takeFirst();
	std::vector<Task_Client*> task_list;
	std::vector<Client*> client_list;
	std::vector<Thread_task*> sendThreadsTask;
	std::vector<Thread_task*> recvThreadsTask;
	int sendThreadCount, recvThreadCount;
};

