#include "stdafx.h"
#include "NetworkTaskManager.h"

void startTh(NetworkTaskManager *ntm);

void TaskList::add_client(Client *c)
{
	std::lock_guard<std::mutex> lock(client_list_lock);
	return_client_list.push_back(c);
}

void TaskList::return_clients(std::vector<Client*> *client_list)
{
	std::lock_guard<std::mutex> lock(client_list_lock);
	if (return_client_list.size() == 0) return;
	client_list->insert(client_list->end(),
		return_client_list.begin(), return_client_list.end());
	return_client_list.clear();
}
Task_Client * TaskList::take_firs()
{
	std::lock_guard<std::mutex> lock(lock_task_list);
	if (taskList.empty()) return nullptr;
	Task_Client *t = taskList.front();
	taskList.erase(taskList.begin());
	task_count--;
	return t;
}

void TaskList::addTask(Task_Client *task)
{
	std::lock_guard<std::mutex> lock(lock_task_list);
	taskList.push_back(task);
	task_count++;
	unlock_thread();
}

int TaskList::get_count()
{
	std::lock_guard<std::mutex> lock(count_lock);
	return task_count;
}

void TaskList::lock_thread()
{
	empty = true;
	std::unique_lock<std::mutex> lock(wait_task);
	while (empty)
	{
		cv.wait(lock);
	}

}

void TaskList::unlock_thread()
{
	std::unique_lock<std::mutex> lock(wait_task);
	empty = false;
	cv.notify_one();
}

void TaskList::thread_end()
{
	unlock_thread();
	work_thread = false;
}

NetworkTaskManager::NetworkTaskManager()
{
	work_server = true;
	sendThreadCount = 2;
	recvThreadCount = 1;
}

NetworkTaskManager::~NetworkTaskManager()
{
}

void NetworkTaskManager::start()
{
	std::thread tm = std::thread(startTh, this);
}

Task_Client *NetworkTaskManager::takeFirst()
{
	std::lock_guard<std::mutex> lock(modifyTaskList);
	Task_Client *rTask = task_list.front();
	task_list.erase(task_list.begin());
	return rTask;
}

void NetworkTaskManager::addTask(Task_Client *t)
{
	std::lock_guard<std::mutex> lock(modifyTaskList);
	task_list.push_back(t);
}

void NetworkTaskManager::return_client_list(std::vector<Client*> *list)
{
	std::lock_guard<std::mutex> lock(control_client_list);
	start_task();
	if (client_list.empty())return;
	list->insert(list->end(), client_list.begin(), client_list.end());
	client_list.clear();
}

void NetworkTaskManager::addTask(CString *str,Client *client)
{
	auto c = firstSubStrDelim(str, L"=", L";");
	Task_Client *t = new Task_Client();
	t->client = client;
	t->task.com = strToCommand(&c);
	if (str->Find(L"Folder")) {
		Dir *d = new Dir(*str);
		t->task.data = (Data*)d;
	}
	else
	{
		File *f = new File(*str);
		t->task.data = (Data*)f;
	}
	addTask(t);
}

void NetworkTaskManager::start_task_manager()
{
	create_thred();
	while (work_server)
	{
		return_client();
		Task_Client *t = takeFirst();
		if (t)
		{
			int *s;
			switch (t->task.com)
			{
			case COMMAND::RE_NAME:
				
				break;
			case COMMAND::DELETE_ITEM:
				break;
			case COMMAND::CREATE_ITEM:
				break;
			case COMMAND::DOWNLOAD:
				s = new int[sendThreadsTask.size()];
				for (int i = 0; i < sendThreadsTask.size(); i++)
				{
					s[i] = sendThreadsTask[i]->all_task.get_count();
				}
				sendThreadsTask[find_optimal_thread(s, sendThreadsTask.size())]->all_task.addTask(t);
				delete[] s;
				break;
			case COMMAND::UPLOAD:
				s = new int[recvThreadsTask.size()];
				for (int i = 0; i < recvThreadsTask.size(); i++)
				{
					s[i] = recvThreadsTask[i]->all_task.get_count();
				}
				recvThreadsTask[find_optimal_thread(s, recvThreadsTask.size())]->all_task.addTask(t);
				delete[] s;
				break;

			default:
				break;
			}
		}
		bool wait;
		for (int i = 0; i < sendThreadsTask.size(); i++)
		{
			if (sendThreadsTask[i]->all_task.get_count() == 0)
			{
				wait = true;
			}
			else
			{
				wait = false;
				break;
			}
		}
		if (wait)
		{
			for (int i = 0; i < recvThreadsTask.size(); i++)
			{
				if (recvThreadsTask[i]->all_task.get_count() == 0)
				{
					wait = true;
				}
				else
				{
					wait = false;
					break;
				}
			}
		}

		if (wait) in_wait_task();
	}
}

void startTh(NetworkTaskManager *ntm)
{
	ntm->start_task_manager();
}

void NetworkTaskManager::return_client()
{
	std::lock_guard<std::mutex> lock(control_client_list);
	for (int i = 0; i < sendThreadsTask.size(); i++)
	{
		sendThreadsTask[i]->all_task.return_clients(&client_list);
	}
	for (int i = 0; i < recvThreadsTask.size(); i++)
	{
		recvThreadsTask[i]->all_task.return_clients(&client_list);
	}
}
void NetworkTaskManager::create_thred()
{
	for (int i = 0; i < sendThreadCount; i++)
	{
		Thread_task *t = new Thread_task();
		t->initSendThread();
		sendThreadsTask.push_back(t);
	}

	for (int i = 0; i < recvThreadCount; i++)
	{
		Thread_task *t = new Thread_task();
		t->initRecvThread();
		recvThreadsTask.push_back(t);
	}
}
void NetworkTaskManager::in_wait_task()
{
	std::unique_lock<std::mutex> lock(wait_task);
	is_task = false;
	while (!is_task)
		cv.wait(lock);
}

void NetworkTaskManager::start_task()
{
	std::unique_lock<std::mutex> lock(wait_task);
	is_task = true;
	cv.notify_one();
}

int NetworkTaskManager::find_optimal_thread(int *count_task, int size)
{
	int min = count_task[0];
	int rVal = 0;
	for (int i = 0; i < size; i++)
	{
		if (count_task[i] < min)
		{
			min = count_task[i];
			rVal = i;
		}
	}
	return rVal;
}

void sendThread(TaskList *task_list)
{
	while (task_list->getWorkThread())
	{
		Task_Client *t = task_list->take_firs();

		if (t)
		{
			if (t->client->sendData(&t->task))
				task_list->add_client(t->client);
			delete t;
		}
		else
		{
			task_list->lock_thread();
		}
	}
}

void recvThread(TaskList *task_list)
{
	while (task_list->getWorkThread())
	{
		Task_Client *t = task_list->take_firs();

		if (t)
		{
			if (t->client->recvData(&t->task))
				task_list->add_client(t->client);
			delete t;
		}
		else
		{
			task_list->lock_thread();
		}
	}
}