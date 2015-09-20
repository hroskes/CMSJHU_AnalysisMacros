make all
./analyzer fileLevel=0 outdir=./ outfile=test.root indir=./ includeGenDecayProb=g1,g4,g4_pi2,m4l includeGenProdProb=g1,g4,g4_pi2,g2,m4l includeRecoDecayProb=g1,g4,g4_pi2,m4l includeRecoProdProb=g1,g4,g4_pi2,g2,m4l computeVBFProdAngles=1 sampleProductionId=JJVBF,JHUGen test.lhe > file.txt

echo '
SelectedTree->Draw("vbf_p0plus_VAJHU","vbf_p0plus_VAJHU==0&&isSelected")
htemp->Integral()
' | root -l -b test.root
