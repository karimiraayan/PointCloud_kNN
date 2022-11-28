#include "simupdatefunctions.h"

int updatelanes(QVector<QRadioButton*> radiobuttons,QVector<QProgressBar*> progressbars,QVector<QVector<LaneStat>> alllanestats,long clock,QVector<long> clockvalues,QVector<double> progresstotal){
    int lastcycles=0;
    int showlastcycles=0;
    bool shownlast = false;
    int median=1;
    for(int i=0; i<radiobuttons.size()-1;i++){ //loop who finds a numer in the text of each radiobutton and setting lastcylces to this value
        if(radiobuttons[i]->isChecked()){
            QRegularExpression rx("[0-9]+");
            QRegularExpressionMatch match = rx.match(radiobuttons[i]->text());
            if(match.hasMatch()){
                lastcycles= match.captured(0).toInt();
            }
        }
    }
    for(int i=0; i<progresstotal.size();i++){
        progresstotal[i]=0;
    }
    int pos=0;
        for(auto it :alllanestats){
            int i=0;
            if(clockvalues.size()>0 && alllanestats.size() >0) {
                if (clockvalues[pos] >= clock - lastcycles) { //only evaluating the lanestats if they are in the last x cycles
                    if (!shownlast) {
                        showlastcycles = clock-clockvalues[pos];
                        shownlast = true;
                    }
                    for (auto k : alllanestats[pos]) { //calculates the relation of NONE to total cycles
                        auto cyclesNone = *k.gettypeCount();
                        auto cycle = cyclesNone.at(CommandVPRO::NONE)[1];
                        int progress = 100*float(clockvalues[pos]-cycle)/float(clockvalues[pos]);
                        progresstotal[i] += progress;
                        i++;
                    }

                median++;
                }
            }
            pos++;
            if(pos>alllanestats.size()){
                 break;
            }
         }
    for(int i=0; i<progresstotal.size();i++){
        progresstotal[i]=progresstotal[i]/median; //divides trough the number of calculations done to get the median
    }
    auto bar =progressbars.begin();
    for(int t=0; t<progresstotal.size(); t++){ //setting the value of the progressbars
         (*bar)->setValue(progresstotal[t]);
         progresstotal[t] =0;
         bar++;
         if(bar == progressbars.end())
               break;
         }

    return showlastcycles;
}

int updatelanestotal(QVector<QProgressBar*> progressbars,long clock,QVector<LaneStat *> lanestats){
    int showlastcycles=0;
    showlastcycles=clock;
    auto bar = progressbars.begin();
    for(auto lanestat : lanestats){
        auto cyclesNone = lanestat->gettypeCount();
        auto cycle = cyclesNone->at(CommandVPRO::NONE)[1];
        int progress = 100*float(clock-cycle)/float(clock);
        (*bar)->setValue(progress);
        bar++;
        if(bar == progressbars.end())
            break;
    }
    return showlastcycles;
}

