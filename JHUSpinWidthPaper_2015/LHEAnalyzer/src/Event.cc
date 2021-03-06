#include <iostream>
#include "../interface/Event.h"

using namespace PDGHelpers;
using namespace ParticleComparators;

void Event::applyParticleSelection(){
  applyLeptonSelection();
  applyNeutrinoSelection();
  applyPhotonSelection();
  applyJetSelection();
  applyZZSelection(); // Order matters here
}
void Event::applyLeptonSelection(){
  for (std::vector<Particle*>::iterator it = leptons.begin(); it<leptons.end(); it++){
    // Trigger and acceptance
    bool passAcceptance = true;
    if (std::abs((*it)->id)==11 && ((*it)->pt()<=electronPTCut || std::abs((*it)->eta())>=electronEtaAcceptanceCut)) passAcceptance = false;
    else if (std::abs((*it)->id)==13 && ((*it)->pt()<=muonPTCut || std::abs((*it)->eta())>=muonEtaAcceptanceCut)) passAcceptance = false;
    else if (std::abs((*it)->id)==15) passAcceptance = false;
    for (std::vector<Particle*>::iterator it2 = leptons.begin(); it2<leptons.end(); it2++){
      if ((*it2)==(*it)) continue; // Every particle is their own ghost.
      else if ((*it)->deltaR((*it2)->p4)<=ghostDeltaRCut) passAcceptance = false; // Ghost removal
    }
    (*it)->setSelected(passAcceptance);
  }
}
void Event::applyNeutrinoSelection(){
  for (std::vector<Particle*>::iterator it = neutrinos.begin(); it<neutrinos.end(); it++) (*it)->setSelected(false);
}
void Event::applyPhotonSelection(){
  for (std::vector<Particle*>::iterator it = photons.begin(); it<photons.end(); it++) (*it)->setSelected(true); // For now...
}
void Event::applyJetSelection(){
  for (std::vector<Particle*>::iterator it = jets.begin(); it<jets.end(); it++){
    bool passAcceptance = true;
    if ((*it)->pt()<=jetPTCut || std::abs((*it)->eta())>=jetEtaAcceptanceCut) passAcceptance = false; // ZZ4l selection and acceptance
    for (std::vector<Particle*>::iterator it2 = leptons.begin(); it2<leptons.end(); it2++){ // Clean from selected leptons
      if ((*it2)->passSelection){ // If it is not selected at all, why would I care?
        if ((*it)->deltaR((*it2)->p4)<=jetDeltaR) passAcceptance = false;
      }
    }
    (*it)->setSelected(passAcceptance);
  }
}
void Event::applyZZSelection(){
  for (std::vector<ZZCandidate*>::iterator it = ZZcandidates.begin(); it<ZZcandidates.end(); it++){
    (*it)->testPreSelectedDaughters();
    if (!(*it)->passSelection) continue;

    bool passAcceptance = true;
    if ((*it)->getSortedV(0)->m()<=mV1LowCut || (*it)->getSortedV(0)->m()>=mV12HighCut){
      passAcceptance = false; (*it)->getSortedV(0)->setSelected(passAcceptance);
    } // Z1 selection
    if ((*it)->getSortedV(1)->m()<=mV2LowCut || (*it)->getSortedV(1)->m()>=mV12HighCut){
      passAcceptance = false; (*it)->getSortedV(1)->setSelected(passAcceptance);
    } // Z2 selection
    for (int iZ=2; iZ<(*it)->getNSortedVs(); iZ++){
      Particle* extraV = (*it)->getSortedV(iZ);
      if (!isAZBoson(extraV->id)) continue;
      else{
        if (extraV->m()<=mllLowCut || extraV->m()>=mV12HighCut || (extraV->getDaughter(0)!=0 && isANeutrino(extraV->getDaughter(0)->id))) extraV->setSelected(false); // Extra Z selection, no effect on ZZ candidate
      }
    }
    TLorentzVector pLOC[2];
    pLOC[0]=(*it)->getAlternativeVMomentum(0);
    pLOC[1]=(*it)->getAlternativeVMomentum(1);
    if (pLOC[0].M()<=mllLowCut || pLOC[1].M()<=mllLowCut) passAcceptance=false;

    (*it)->setSelected(passAcceptance);
  }
}

void Event::addZZCandidate(ZZCandidate* myParticle){
  bool isIdentical = (getNZZCandidates()>0);
  for (int cc=0; cc<getNZZCandidates(); cc++){
    ZZCandidate* testCand = ZZcandidates.at(cc);

    for (int i=0; i<2; i++){
      Particle* testV = testCand->getSortedV(i);
      Particle* partV = myParticle->getSortedV(i);
      if (partV==0 || testV==0){
        if (partV==0 && testV==0) continue; // Check the next intermediate V if there is any.
        else { isIdentical=false; break; } // They are definitely not the same.
      }
      if (testV->getNDaughters() != partV->getNDaughters()) { isIdentical=false;  break; }; // Again, they cannot be the same.
      for (int j=0; j<testV->getNDaughters(); j++){
        Particle* testD = testV->getDaughter(j);
        Particle* partD = partV->getDaughter(j);
        isIdentical = isIdentical && (testD==partD);
        if (!isIdentical) break;
      }
      if (!isIdentical) break;
    }
    if (!isIdentical) break;
  }
  if (!isIdentical) ZZcandidates.push_back(myParticle);
  else { delete myParticle; myParticle=0; }
}

void Event::constructVVCandidates(int isZZ, int fstype){
  /*
  fstype  / ZZ==1 / WW==0  / Yukawa==2 / Zgam=3 / gamgam=4 / Z+nj=5
  fstype=0: 4l    / lnulnu / 2l        / 2l     / gam      / 2l
  fstype=1: 4q    / 4q     / 2q        / 2q     / -        / 2q
  fstype=2: 2l2q  / lnu2q  / -         / -      / -        / -
  fstype=3: 2l2nu / -      / -         / -      / -        / -
  fstype=4: 2q2nu / -      / -         / -      / -        / -
  fstype=5: 4nu   / -      / -         / 2nu    / -        / 2nu
  fstype=-1: Any
  */

  if (
    (isZZ<=0 && fstype>2)
    ||
    (isZZ==1 && fstype>5)
    ||
    (isZZ==2 && fstype>1)
    ||
    (isZZ==3 && (fstype>0 && fstype!=5))
    ||
    (isZZ==4 && fstype>1)
    ||
    (isZZ==5 && (fstype>1 && fstype!=5))
    ||
    isZZ>5
    ){
    if (isZZ<0) std::cerr << "No " << "undecayed" << " candidate with final state " << fstype << " is possible!" << std::endl;
    else if (isZZ==0) std::cerr << "No " << "WW" << " candidate with final state " << fstype << " is possible!" << std::endl;
    else if (isZZ==1) std::cerr << "No " << "ZZ" << " candidate with final state " << fstype << " is possible!" << std::endl;
    else if (isZZ==2) std::cerr << "No " << "f-fbar" << " candidate with final state " << fstype << " is possible!" << std::endl;
    else if (isZZ==3) std::cerr << "No " << "Zgamma" << " candidate with final state " << fstype << " is possible!" << std::endl;
    else if (isZZ==4) std::cerr << "No " << "gammagamma" << " candidate with final state " << fstype << " is possible!" << std::endl;
    else if (isZZ==5) std::cerr << "No " << "Z+(n)jets" << " candidate with final state " << fstype << " is possible!" << std::endl;
    else if (isZZ>5) std::cerr << "Unknown candidate with final state " << fstype << "!" << std::endl;
    return;
  }

  std::vector<Particle*> lepMinusPlus[3][2]; // l-, l+
  std::vector<Particle*> lepNuNubar[3][2]; // nu, nub
  std::vector<Particle*> quarkAntiquark[7][2]; // q, qb

  for (std::vector<Particle*>::iterator it = leptons.begin(); it<leptons.end(); it++){ // Leptons
    int iFirst=0;
    int iSecond=0;

    if (abs((*it)->id)==11) iFirst = 0;
    else if (abs((*it)->id)==13) iFirst = 1;
    else if (abs((*it)->id)==15) iFirst = 2;
    if ((*it)->id<0) iSecond=1;
    lepMinusPlus[iFirst][iSecond].push_back(*it);
  }
  for (std::vector<Particle*>::iterator it = neutrinos.begin(); it<neutrinos.end(); it++){ // Neutrinos
    int iFirst=0;
    int iSecond=0;

    if (abs((*it)->id)==12) iFirst = 0;
    else if (abs((*it)->id)==14) iFirst = 1;
    else if (abs((*it)->id)==16) iFirst = 2;
    if ((*it)->id<0) iSecond=1;
    lepNuNubar[iFirst][iSecond].push_back(*it);
  }
  for (std::vector<Particle*>::iterator it = jets.begin(); it<jets.end(); it++){ // Jets
    int iFirst=abs((*it)->id); // Yes, 0-6, 0 being unknown
    if (PDGHelpers::isAGluon(iFirst)) continue;
    int iSecond=0;

    if ((*it)->id<0) iSecond=1;
    quarkAntiquark[iFirst][iSecond].push_back(*it);
  }

  std::vector<Particle*> tmpVhandle;

  if (isZZ==1 || isZZ==3 || isZZ==5){ // ZZ

    if (fstype==-1 || (isZZ==1 && (fstype==0 || fstype==2 || fstype==3)) || (isZZ==3 && fstype==0)){ // Z->2l
      for (int c=0; c<3; c++){
        for (unsigned int i=0; i<lepMinusPlus[c][0].size(); i++){
          for (unsigned int j=0; j<lepMinusPlus[c][1].size(); j++){
            TLorentzVector pV = lepMinusPlus[c][0].at(i)->p4+lepMinusPlus[c][1].at(j)->p4;
            Particle* V = new Particle(23, pV);
            V->addDaughter(lepMinusPlus[c][0].at(i));
            V->addDaughter(lepMinusPlus[c][1].at(j));
            tmpVhandle.push_back(V);
          }
        }
      }
    }
    if (fstype==-1 || (isZZ==1 && (fstype==3 || fstype==4 || fstype==5)) || (isZZ==3 && fstype==5)){ // Z->2nu
      for (int c=0; c<3; c++){
        for (unsigned int i=0; i<lepNuNubar[c][0].size(); i++){
          for (unsigned int j=0; j<lepNuNubar[c][1].size(); j++){
            TLorentzVector pV = lepNuNubar[c][0].at(i)->p4+lepNuNubar[c][1].at(j)->p4;
            Particle* V = new Particle(23, pV);
            V->addDaughter(lepNuNubar[c][0].at(i));
            V->addDaughter(lepNuNubar[c][1].at(j));
            tmpVhandle.push_back(V);
          }
        }
      }
    }
    if (fstype==-1 || (isZZ==1 && (fstype==1 || fstype==2 || fstype==4)) || (isZZ==3 && fstype==1)){ // Z->2q
      for (int c=1; c<7; c++){
        for (unsigned int i=0; i<quarkAntiquark[c][0].size(); i++){
          for (unsigned int j=0; j<quarkAntiquark[c][1].size(); j++){
            TLorentzVector pV = quarkAntiquark[c][0].at(i)->p4+quarkAntiquark[c][1].at(j)->p4;
            Particle* V = new Particle(23, pV);
            V->addDaughter(quarkAntiquark[c][0].at(i));
            V->addDaughter(quarkAntiquark[c][1].at(j));
            tmpVhandle.push_back(V);
          }
        }
      }
    }

  }
  else if(isZZ==0){ // WW

    if (fstype==-1 || fstype==0 || fstype==2){ // W->lnu
      for (int c=0; c<3; c++){
        for (unsigned int i=0; i<lepMinusPlus[c][1].size(); i++){
          for (unsigned int j=0; j<lepNuNubar[c][0].size(); j++){
            TLorentzVector pV = lepMinusPlus[c][1].at(i)->p4+lepNuNubar[c][0].at(j)->p4;
            Particle* V = new Particle(24, pV);
            V->addDaughter(lepMinusPlus[c][1].at(i));
            V->addDaughter(lepNuNubar[c][0].at(j));
            tmpVhandle.push_back(V);
          }
        }
        for (unsigned int i=0; i<lepMinusPlus[c][0].size(); i++){
          for (unsigned int j=0; j<lepNuNubar[c][1].size(); j++){
            TLorentzVector pV = lepMinusPlus[c][0].at(i)->p4+lepNuNubar[c][1].at(j)->p4;
            Particle* V = new Particle(-24, pV);
            V->addDaughter(lepMinusPlus[c][0].at(i));
            V->addDaughter(lepNuNubar[c][1].at(j));
            tmpVhandle.push_back(V);
          }
        }
      }
    }
    if (fstype==-1 || fstype==1 || fstype==2){ // W->2q
      for (int c=1; c<7; c++){
        for (int d=1; d<7; d++){
          if (d==c) continue;
          for (unsigned int i=0; i<quarkAntiquark[c][0].size(); i++){
            for (unsigned int j=0; j<quarkAntiquark[d][1].size(); j++){
              int totalcharge = quarkAntiquark[c][0].at(i)->charge() + quarkAntiquark[d][1].at(j)->charge();
              if (abs(totalcharge)!=1) continue;

              TLorentzVector pV = quarkAntiquark[c][0].at(i)->p4+quarkAntiquark[d][1].at(j)->p4;
              Particle* V = new Particle(24*totalcharge, pV);
              V->addDaughter(quarkAntiquark[c][0].at(i));
              V->addDaughter(quarkAntiquark[d][1].at(j));
              tmpVhandle.push_back(V);
            }
          }
        }
      }
    }
  }
  else if (isZZ==2){ // H->f fbar

    if (fstype==-1 || fstype==0){ // H->2l
      for (int c=0; c<3; c++){
        for (unsigned int i=0; i<lepMinusPlus[c][0].size(); i++){
          for (unsigned int j=0; j<lepMinusPlus[c][1].size(); j++){
            Particle* F1 = lepMinusPlus[c][0].at(i);
            Particle* F2 = lepMinusPlus[c][1].at(j);

            TLorentzVector pH = F1->p4+F2->p4;
            ZZCandidate* cand = new ZZCandidate(25, pH);
            cand->addDaughter(F1);
            cand->addDaughter(F2);

            double defaultHVVmass = HVVmass;
            setHVVmass(Zeromass);
            cand->sortDaughters();
            setHVVmass(defaultHVVmass);
            addZZCandidate(cand);
          }
        }
      }
    }
    if (fstype==-1 || fstype==1){ // H->2q
      for (int c=1; c<7; c++){
        for (unsigned int i=0; i<quarkAntiquark[c][0].size(); i++){
          for (unsigned int j=0; j<quarkAntiquark[c][1].size(); j++){
            Particle* F1 = quarkAntiquark[c][0].at(i);
            Particle* F2 = quarkAntiquark[c][1].at(j);

            TLorentzVector pH = F1->p4+F2->p4;
            ZZCandidate* cand = new ZZCandidate(25, pH);
            cand->addDaughter(F1);
            cand->addDaughter(F2);

            double defaultHVVmass = HVVmass;
            setHVVmass(Zeromass);
            cand->sortDaughters();
            setHVVmass(defaultHVVmass);
            addZZCandidate(cand);
          }
        }
      }
    }

  }
  else if (isZZ==5){ // Z->f fbar

    if (fstype==-1 || fstype==0){ // Z->2l
      for (int c=0; c<3; c++){
        for (unsigned int i=0; i<lepMinusPlus[c][0].size(); i++){
          for (unsigned int j=0; j<lepMinusPlus[c][1].size(); j++){
            Particle* F1 = lepMinusPlus[c][0].at(i);
            Particle* F2 = lepMinusPlus[c][1].at(j);

            TLorentzVector pCand = F1->p4+F2->p4;
            ZZCandidate* cand = new ZZCandidate(23, pCand);
            cand->addDaughter(F1);
            cand->addDaughter(F2);

            double defaultHVVmass = HVVmass;
            setHVVmass(Zeromass);
            cand->sortDaughters();
            setHVVmass(defaultHVVmass);
            addZZCandidate(cand);
          }
        }
      }
    }
    if (fstype==-1 || fstype==1){ // Z->2q
      for (int c=1; c<7; c++){
        for (unsigned int i=0; i<quarkAntiquark[c][0].size(); i++){
          for (unsigned int j=0; j<quarkAntiquark[c][1].size(); j++){
            Particle* F1 = quarkAntiquark[c][0].at(i);
            Particle* F2 = quarkAntiquark[c][1].at(j);

            TLorentzVector pCand = F1->p4+F2->p4;
            ZZCandidate* cand = new ZZCandidate(23, pCand);
            cand->addDaughter(F1);
            cand->addDaughter(F2);

            double defaultHVVmass = HVVmass;
            setHVVmass(Zeromass);
            cand->sortDaughters();
            setHVVmass(defaultHVVmass);
            addZZCandidate(cand);
          }
        }
      }
    }
    if (fstype==-1 || fstype==5){ // Z->2nu
      for (int c=0; c<3; c++){
        for (unsigned int i=0; i<lepNuNubar[c][0].size(); i++){
          for (unsigned int j=0; j<lepNuNubar[c][1].size(); j++){
            Particle* F1 = lepNuNubar[c][0].at(i);
            Particle* F2 = lepNuNubar[c][1].at(j);

            TLorentzVector pCand = F1->p4+F2->p4;
            ZZCandidate* cand = new ZZCandidate(23, pCand);
            cand->addDaughter(F1);
            cand->addDaughter(F2);

            double defaultHVVmass = HVVmass;
            setHVVmass(Zeromass);
            cand->sortDaughters();
            setHVVmass(defaultHVVmass);
            addZZCandidate(cand);
          }
        }
      }
    }

  }
  else{ // Undecayed
    for (std::vector<Particle*>::iterator it = intermediates.begin(); it<intermediates.end(); it++){ // Add directly
      if (isAHiggs((*it)->id)){
        TLorentzVector pH = (*it)->p4;
        ZZCandidate* cand = new ZZCandidate(25, pH);

        double defaultHVVmass = HVVmass;
        setHVVmass(Zeromass);
        cand->sortDaughters();
        setHVVmass(defaultHVVmass);
        addZZCandidate(cand);
      }
    }
  }

  if (debugVars::debugFlag) std::cout << "Number of V/ZZ before sorting photons: " << tmpVhandle.size() << " " << getNZZCandidates() << std::endl;

  if (isZZ==3 || isZZ==4){
    for (int i=0; i<photons.size(); i++){ // Copy the photons
      TLorentzVector pV = photons.at(i)->p4;
      Particle* V = new Particle(photons.at(i)->id, pV);
      V->addDaughter(photons.at(i)); // Photon is its own daughter!
      tmpVhandle.push_back(V);
    }
  }

  if (debugVars::debugFlag) std::cout << "Number of V/ZZ after sorting photons: " << tmpVhandle.size() << " " << getNZZCandidates() << std::endl;

  if (
    ((fstype==-1 || fstype==1 || fstype==2 || fstype==4) && (isZZ==0 || isZZ==1)) // W/Z->2j reco.-level
    ||
    ((fstype==-1 || fstype==1) && isZZ==2) // H->2j reco.-level
    ||
    ((fstype==-1 || fstype==1) && isZZ==3) // H->Zgam with Z->2j
    ||
    ((fstype==-1 || fstype==1) && isZZ==5) // Z+2jets with Z->2j
    ){
    for (unsigned int i=0; i<quarkAntiquark[0][0].size(); i++){
      if (quarkAntiquark[0][0].at(i)->id!=0) continue;
      for (unsigned int j=i+1; j<quarkAntiquark[0][0].size(); j++){
        if (quarkAntiquark[0][0].at(j)->id!=0) continue;
        if (isZZ==0 || isZZ==1 || isZZ==3){
          TLorentzVector pV = quarkAntiquark[0][0].at(i)->p4+quarkAntiquark[0][0].at(j)->p4;
          Particle* V = new Particle(0, pV);
          V->addDaughter(quarkAntiquark[0][0].at(i));
          V->addDaughter(quarkAntiquark[0][0].at(j));
          tmpVhandle.push_back(V);
        }
        else if (isZZ==2){
          Particle* F1 = quarkAntiquark[0][0].at(i);
          Particle* F2 = quarkAntiquark[0][0].at(j);

          TLorentzVector pH = F1->p4+F2->p4;
          ZZCandidate* cand = new ZZCandidate(25, pH);
          cand->addDaughter(F1);
          cand->addDaughter(F2);

          double defaultHVVmass = HVVmass;
          setHVVmass(Zeromass);
          cand->sortDaughters();
          setHVVmass(defaultHVVmass);
          addZZCandidate(cand);
        }
        else if (isZZ==5){
          Particle* F1 = quarkAntiquark[0][0].at(i);
          Particle* F2 = quarkAntiquark[0][0].at(j);

          TLorentzVector pCand = F1->p4+F2->p4;
          ZZCandidate* cand = new ZZCandidate(23, pCand);
          cand->addDaughter(F1);
          cand->addDaughter(F2);

          double defaultHVVmass = HVVmass;
          setHVVmass(Zeromass);
          cand->sortDaughters();
          setHVVmass(defaultHVVmass);
          addZZCandidate(cand);
        }
      }
    }
    if (debugVars::debugFlag) std::cout << "Number of V/ZZ after sorting reco. jets: " << tmpVhandle.size() << " " << getNZZCandidates() << std::endl;
  }



  for (unsigned int i=0; i<tmpVhandle.size(); i++){
    for (unsigned int j=i; j<tmpVhandle.size(); j++){
      if (tmpVhandle.at(i)==tmpVhandle.at(j)) continue;
      if ((tmpVhandle.at(i)->charge()+tmpVhandle.at(j)->charge())!=0) continue;

      Particle* Vi1 = tmpVhandle.at(i)->getDaughter(0);
      Particle* Vi2 = tmpVhandle.at(i)->getDaughter(1);
      Particle* Vj1 = tmpVhandle.at(j)->getDaughter(0);
      Particle* Vj2 = tmpVhandle.at(j)->getDaughter(1);
      /*
      std::cout << "11: " << Vi1->id << '\t' << Vi1->x() << '\t' << Vi1->y() << '\t' << Vi1->z() << '\t' << Vi1->t() << '\t' << std::endl;
      std::cout << "12: " << Vi2->id << '\t' << Vi2->x() << '\t' << Vi2->y() << '\t' << Vi2->z() << '\t' << Vi2->t() << '\t' << std::endl;
      std::cout << "21: " << Vj1->id << '\t' << Vj1->x() << '\t' << Vj1->y() << '\t' << Vj1->z() << '\t' << Vj1->t() << '\t' << std::endl;
      std::cout << "22: " << Vj2->id << '\t' << Vj2->x() << '\t' << Vj2->y() << '\t' << Vj2->z() << '\t' << Vj2->t() << '\t' << std::endl;
      */
      if (Vi1==Vj1 || (Vi2==Vj2 && Vi2 != 0)) continue;

      if (debugVars::debugFlag){
        if (Vi1!=0) std::cout << "Vi1 not zero. Id: " << Vi1->id << std::endl;
        if (Vi2!=0) std::cout << "Vi2 not zero. Id: " << Vi2->id << std::endl;
        if (Vj1!=0) std::cout << "Vj1 not zero. Id: " << Vj1->id << std::endl;
        if (Vj2!=0) std::cout << "Vj2 not zero. Id: " << Vj2->id << std::endl;
      }

      TLorentzVector pH(0, 0, 0, 0);
      if (Vi1!=0) pH = pH + Vi1->p4;
      if (Vi2!=0) pH = pH + Vi2->p4;
      if (Vj1!=0) pH = pH + Vj1->p4;
      if (Vj2!=0) pH = pH + Vj2->p4;
      ZZCandidate* cand = new ZZCandidate(25, pH);

      if (Vi1!=0) cand->addDaughter(Vi1);
      if (Vi2!=0) cand->addDaughter(Vi2);
      if (Vj1!=0) cand->addDaughter(Vj1);
      if (Vj2!=0) cand->addDaughter(Vj2);

      double defaultHVVmass = HVVmass;
      if (isZZ==0) setHVVmass(Wmass);
      else if (isZZ!=4) setHVVmass(Zmass);
      else setHVVmass(Zeromass);

      if (debugVars::debugFlag) std::cout << "Sorting daughters..." << std::endl;
      cand->sortDaughters();
      if (debugVars::debugFlag) std::cout << "Sorted daughters successfully!" << std::endl;
      setHVVmass(defaultHVVmass);

      addZZCandidate(cand);

      if (debugVars::debugFlag) std::cout << "Added candidate for V" << i << " V" << j << std::endl;
    }
  }

  for (unsigned int i=0; i<tmpVhandle.size(); i++) delete tmpVhandle.at(i);
  if (debugVars::debugFlag) std::cout << "tmpVhandle deletion step is done." << std::endl;
  tmpVhandle.clear();
}

ZZCandidate* Event::getZZCandidate(int index)const{
  if ((int)ZZcandidates.size()>index) return ZZcandidates.at(index);
  else return 0;
}
Particle* Event::getLepton(int index)const{
  if ((int)leptons.size()>index) return leptons.at(index);
  else return 0;
}
Particle* Event::getNeutrino(int index)const{
  if ((int)neutrinos.size()>index) return neutrinos.at(index);
  else return 0;
}
Particle* Event::getPhoton(int index)const{
  if ((int)photons.size()>index) return photons.at(index);
  else return 0;
}Particle* Event::getJet(int index)const{
  if ((int)jets.size()>index) return jets.at(index);
  else return 0;
}
Particle* Event::getIntermediate(int index)const{
  if ((int)intermediates.size()>index) return intermediates.at(index);
  else return 0;
}
Particle* Event::getParticle(int index)const{
  if ((int)particles.size()>index) return particles.at(index);
  else return 0;
}

TLorentzVector Event::missingP() const{
  TLorentzVector totalP(0, 0, 0, 0);
  for (unsigned int pp=0; pp<particles.size(); pp++){
    Particle* part = getParticle(pp);
    if (part->passSelection) totalP = totalP + part->p4;
  }
  totalP.SetT(totalP.P());
  totalP.SetVect(-totalP.Vect());
  return totalP;
}

void Event::addVVCandidateMother(Particle* mother){
  for (std::vector<ZZCandidate*>::iterator it = ZZcandidates.begin(); it<ZZcandidates.end(); it++) (*it)->addMother(mother);
}
void Event::addVVCandidateAppendages(){
  for (std::vector<ZZCandidate*>::iterator it = ZZcandidates.begin(); it<ZZcandidates.end(); it++){
    for (std::vector<Particle*>::iterator iL = leptons.begin(); iL<leptons.end(); iL++){ if ((*iL)->passSelection) (*it)->addAssociatedLeptons(*iL); }
    for (std::vector<Particle*>::iterator iL = neutrinos.begin(); iL<neutrinos.end(); iL++){ if ((*iL)->passSelection) (*it)->addAssociatedNeutrinos(*iL); }
    for (std::vector<Particle*>::iterator iP = photons.begin(); iP<photons.end(); iP++){ if ((*iP)->passSelection) (*it)->addAssociatedPhotons(*iP); }
    for (std::vector<Particle*>::iterator iJ = jets.begin(); iJ<jets.end(); iJ++){ if ((*iJ)->passSelection) (*it)->addAssociatedJets(*iJ); }
    (*it)->addAssociatedVs();
  }
}





