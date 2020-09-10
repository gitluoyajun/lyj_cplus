#include <cstdio>
#include "../Common/templateInterface.h"
int main()
{
    printf("hello from TemplateInterFace!\n");
    IService* service = new Service(new Client());
    service->SendMessage("hello template!");
    return 0;
}