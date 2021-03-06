#ifndef EVENTBASE_H
#define EVENTBASE_H

#include <vector>
#include "TLorentzVector.h"
#include "ZZCandidate.h"
#include "ParticleComparators.h"

class Event{
public:

  // Constructors

  Event() : weight(0),xsec(0){}
  ~Event(){ wipeAll(); }

  // Data

  // Member functions
  void setWeight(double weight_){ weight=weight_; }
  void addExtraWeight(double weight_){ extraWeight.push_back(weight_); }
  int getNExtraWeights() const{ return extraWeight.size(); }
  double getWeight(int index=-1) const{ if (index==-1) return weight; else if (index<getNExtraWeights()) return extraWeight.at(index); else return 0; }
  std::vector<double>& getExtraWeights(){ return extraWeight; }
  void setXSec(double xsec_){ xsec=xsec_; }
  double getXSec() const{ return xsec; }


  void constructVVCandidates(int isZZ=1, int fstype=0);
  void applyParticleSelection();
  void addVVCandidateMother(Particle* mother);
  void addVVCandidateAppendages();


  int getNZZCandidates() const{ return ZZcandidates.size(); }
  int getNLeptons() const{ return leptons.size(); }
  int getNNeutrinos() const{ return neutrinos.size(); }
  int getNPhotons() const{ return photons.size(); }
  int getNJets() const{ return jets.size(); }
  int getNIntermediates() const{ return intermediates.size(); }
  int getNParticles() const{ return particles.size(); }

  ZZCandidate* getZZCandidate(int index) const;
  Particle* getLepton(int index) const;
  Particle* getNeutrino(int index) const;
  Particle* getPhoton(int index) const;
  Particle* getJet(int index) const;
  Particle* getIntermediate(int index) const;
  Particle* getParticle(int index) const;

  void addParticle(Particle* myParticle){ particles.push_back(myParticle); }
  void addIntermediate(Particle* myParticle){ intermediates.push_back(myParticle); }
  void addLepton(Particle* myParticle, bool genuineParticle=true){ leptons.push_back(myParticle); if (genuineParticle) addParticle(myParticle); }
  void addNeutrino(Particle* myParticle, bool genuineParticle=true){ neutrinos.push_back(myParticle); if (genuineParticle) addParticle(myParticle); }
  void addPhoton(Particle* myParticle, bool genuineParticle=true){ photons.push_back(myParticle); if (genuineParticle) addParticle(myParticle); }
  void addJet(Particle* myParticle, bool genuineParticle=true){ jets.push_back(myParticle); if (genuineParticle) addParticle(myParticle); }
  TLorentzVector missingP() const;

protected:
  std::vector<Particle*> particles;
  std::vector<Particle*> intermediates;
  std::vector<Particle*> leptons;
  std::vector<Particle*> neutrinos;
  std::vector<Particle*> photons;
  std::vector<Particle*> jets;
  std::vector<ZZCandidate*> ZZcandidates;

  void addZZCandidate(ZZCandidate* myParticle); // Protected to avoid adding external ZZCandidates and DELETING THEM TWICE!

  template<typename ParticleType> void wipeArray(std::vector<ParticleType*>& particleArray, bool doDelete=true){ if (doDelete){ for (unsigned int i=0; i<particleArray.size(); i++){ ParticleType* delpar = particleArray.at(i); delete delpar; } } particleArray.clear(); };
  void wipeAll(){ leptons.clear(); neutrinos.clear(); photons.clear(); jets.clear(); wipeArray(ZZcandidates, true); wipeArray(intermediates, false); wipeArray(particles, false); };

  void applyLeptonSelection();
  void applyNeutrinoSelection();
  void applyPhotonSelection();
  void applyJetSelection();
  void applyZZSelection();

  double xsec;
  double weight;
  std::vector<double> extraWeight;
};


#endif
