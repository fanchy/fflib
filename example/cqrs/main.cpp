
#include <iostream>
using namespace std;

#include "cqrs/bus_i.h"
#include "user/user_mgr.h"
#include "command/task_command.h"
#include "task/task_service.h"
#include "task/test_task.h"
#include  "base/obj_tool.h"
using namespace ff;

#include "gtest/gtest.h"

int main(int argc, char* argv[])
{
    TASK_SERVICE.start();
    
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
    return 0;
}

/*
class monster_t
{
protected:
	player_t* m_attack;

public:
	void handle_ai()
	{
		if (m_attack)
		{
			int x = m_attack->get_x();
		}
	}
}

class monster_t
{
protected:
	long m_attack_id;

public:
	void handle_ai()
	{
		player_t* attack = obj_mgr.get(m_attack_id);
		if (attack)
		{
			int x = attack->get_x();
		}
	}
}


class monster_t
{
protected:
	player_t* m_attack;

public:
	void handle_ai()
	{
		if (obj_mgr.is_exist(m_attack))
		{
			int x = m_attack->get_x();
		}
	}
}



class monster_t
{
protected:
	weak_ptr<player_t> m_attack;
	shared_ptr<player_t> get_attack()
	{
		return shared_ptr<player_t>(m_attack);
	}
public:
	void handle_ai()
	{
		shared_ptr<player_t> attack = get_attack();
		if (attack)
		{
			int x = attack->get_x();
		}
	}
}
*/



