#include "commandwindow.h"
#include <QApplication>
#include <QDebug>
#include <QLabel>
#include "../../../model/commands/CommandVPRO.h"
#include "../../../model/commands/CommandDMA.h"
#include "../../../model/commands/CommandSim.h"
#include <QMetaType>

#include <memory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    QVector<QVector<std::shared_ptr<CommandVPRO>>> command_queue;
    QVector<std::shared_ptr<CommandVPRO>> commands;

    for(int x=0; x<4; x++){
        QVector<std::shared_ptr<CommandVPRO>> cmd_queuevector;
        for(int i=0; i<4;i++){
            cmd_queuevector.push_back(std::make_shared<CommandVPRO>());
        }
        command_queue.push_back(cmd_queuevector);
    }
    for(int y=0; y<8; y++){
        commands.push_back(std::make_shared<CommandVPRO>());
        commands.push_back(std::make_shared<CommandVPRO>());
    }

    CommandWindow::Data data;

    CommandWindow w(1,2);

    w.dataUpdate(data);
    w.simUpdate(5);
    uint8_t *lm = new uint8_t[8192*2*2]();

    lm[0] = 0xff;
    lm[1] = 0xff;
    lm[3] = 0xff;

    w.setLocalMemory(0, &(lm[0]), 0);
    w.setLocalMemory(1, &(lm[8192*2]), 0);

    w.show();

    return a.exec();
}
