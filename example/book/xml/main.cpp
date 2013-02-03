
#include "xml/ffxml.h"
using namespace ff;


//<game type = "good">
//    <scene>happly</scene>
//    <role ID="123456"  pos = "any">
//        <name nick = "xx" >OhNice</name>
//        <num>99</num>
//    </role>
//</game>

int main(int argc, char* argv[])
{
    ffxml_t ffxml;
    
    //! 载入test.xml
    if (ffxml.load("test.xml"))
    {
        printf("test.xml 载入失败\n");
        return 1;
    }

    printf("获取字段     game.scene:        %s\n", ffxml.get("game.scene"));
    printf("获取字段     game.role.name:    %s\n", ffxml.get("game.role.name"));
    printf("获取字段     game.role.num:     %s\n", ffxml.get("game.role.num"));
    
    printf("获取属性     game.{type}:       %s\n", ffxml.get("game.{type}"));
    printf("获取属性     game.role.{ID}:    %s\n", ffxml.get("game.role.{ID}"));
    
    printf("获取标记数量 game:              %u\n", ffxml.size("game"));
    printf("获取标记数量 game.role:         %u\n", ffxml.size("game.role"));
    
    printf("获取属性数量 game:              %u\n", ffxml.size("game.{}"));
    printf("获取属性数量 game.role:         %u\n", ffxml.size("game.role.{}"));
    
    //! 遍历子节点
    char arg_key[128];
    char arg_val[128];
    for (size_t i = 0; i < ffxml.size("game.role"); ++i)
    {
        sprintf(arg_key, "game.role.&%u", i);
        sprintf(arg_val, "game.role.@%u", i);
        printf("遍历子节点   game.role:         %s->%s\n", ffxml.get(arg_key), ffxml.get(arg_val));
    }
    
    //! 遍历属性节点
    for (size_t i = 0; i < ffxml.size("game.role"); ++i)
    {
        sprintf(arg_key, "game.role.{&%u}", i);
        sprintf(arg_val, "game.role.{@%u}", i);
        printf("遍历属性     game.role:         %s->%s\n", ffxml.get(arg_key), ffxml.get(arg_val));
    }
    
    
    printf("组合         game.role.@1.{@nick} %s\n", ffxml.get("game.role.@0.{@nick}"));
    return 0;
}
