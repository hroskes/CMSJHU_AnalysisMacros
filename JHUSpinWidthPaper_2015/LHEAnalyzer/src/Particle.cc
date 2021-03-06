#include "../interface/Particle.h"

using namespace PDGHelpers;

namespace debugVars{
  bool debugFlag = false;
}


Particle::Particle():
id(0),
passSelection(true),
genStatus(-2),
lifetime(0)
{
  p4.SetXYZT(0, 0, 0, 0);
}

Particle::Particle(int id_, TLorentzVector p4_) :
id(id_),
passSelection(true),
genStatus(-2),
lifetime(0)
{
  p4.SetXYZT(p4_.X(), p4_.Y(), p4_.Z(), p4_.T());
}
Particle::Particle(const Particle& particle_) :
id(particle_.id),
passSelection(particle_.passSelection),
genStatus(particle_.genStatus),
lifetime(particle_.lifetime)
{
  p4.SetXYZT(particle_.p4.X(), particle_.p4.Y(), particle_.p4.Z(), particle_.p4.T());
  for (int index=0; index<particle_.getNMothers(); index++) addMother(particle_.getMother(index));
  for (int index=0; index<particle_.getNDaughters(); index++) addDaughter(particle_.getDaughter(index));
}
Particle& Particle::operator=(const Particle& particle_){
  id=particle_.id;
  passSelection=particle_.passSelection;
  genStatus=particle_.genStatus;
  lifetime=particle_.lifetime;
  for (int index=0; index<particle_.getNMothers(); index++) addMother(particle_.getMother(index));
  for (int index=0; index<particle_.getNDaughters(); index++) addDaughter(particle_.getDaughter(index));
  return *this;
}


bool Particle::checkParticleExists(Particle* myParticle, std::vector<Particle*>& particleArray){
  for (std::vector<Particle*>::iterator it = particleArray.begin(); it<particleArray.end(); it++){
    if ((*it)==myParticle) return true;
  }
  return false;
}
void Particle::addMother(Particle* myParticle){ if (!checkParticleExists(myParticle, mothers)) mothers.push_back(myParticle); }
void Particle::addDaughter(Particle* myParticle){ if (!checkParticleExists(myParticle, daughters)) daughters.push_back(myParticle); }
Particle* Particle::getMother(int index)const{
  if ((int)mothers.size()>index) return mothers.at(index);
  else return 0;
}
Particle* Particle::getDaughter(int index)const{
  if ((int)daughters.size()>index) return daughters.at(index);
  else return 0;
}
std::vector<int> Particle::getDaughterIds()const{
  std::vector<int> result;
  for (unsigned int idau=0; idau<daughters.size(); idau++){
    if (daughters.at(idau)!=0) result.push_back(daughters.at(idau)->id);
  }
  return result;
}

double Particle::charge()const{
  double cpos=0;
  if (isAWBoson(id) || abs(id)==37 || abs(id)==2212 || abs(id)==211 || abs(id)==321 || abs(id)==411 || abs(id)==521) cpos = 1.;
  else if (isALepton(id)) cpos = -1.;
  else if (isUpTypeQuark(id)) cpos = 2./3.;
  else if (isDownTypeQuark(id)) cpos = -1./3.;
  if (id<0) cpos *= -1.;
  return cpos;
}



