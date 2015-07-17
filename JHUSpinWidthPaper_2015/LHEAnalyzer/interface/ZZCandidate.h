#ifndef ZZCANDIDATE_H
#define ZZCANDIDATE_H

#include "Particle.h"

class ZZCandidate : public Particle{
public:
  ZZCandidate(int id_, TLorentzVector p4_) : Particle(id_, p4_) {};

  ~ZZCandidate(){ for (int i=0; i<sortedVs.size(); i++) delete sortedVs.at(i); sortedVs.clear(); sortedDaughters.clear(); associatedJets.clear(); associatedLeptons.clear(); };


  // Member functions

  Particle* getSortedDaughter(int index)const;
  Particle* getSortedV(int index)const;
  Particle* getAssociatedLepton(int index)const;
  Particle* getAssociatedNeutrino(int index)const;
  Particle* getAssociatedJet(int index)const;

  int getNAssociatedLeptons()const{ return associatedLeptons.size(); };
  int getNAssociatedNeutrinos()const{ return associatedNeutrinos.size(); };
  int getNAssociatedJets()const{ return associatedJets.size(); };
  int getNSortedVs()const{ return sortedVs.size(); };

  void addAssociatedLeptons(Particle* myParticle);
  void addAssociatedNeutrinos(Particle* myParticle);
  void addAssociatedJets(Particle* myParticle);
  void addSortedV(Particle* myParticle){ sortedVs.push_back(myParticle); };
  void addAssociatedVs();

  void sortDaughters();
  void testPreSelectedLeptons();

private:
  std::vector<Particle*> associatedLeptons;
  std::vector<Particle*> associatedNeutrinos;
  std::vector<Particle*> associatedJets;

  std::vector<Particle*> sortedDaughters;
  std::vector<Particle*> sortedVs;


  void sortDaughtersInitial();
  void sortDaughtersByBestZ1();
  void createSortedVs();
  void createAssociatedVs(std::vector<Particle*>& particleArray);
  void addByHighestPt(Particle* myParticle, std::vector<Particle*>& particleArray);
};



#endif