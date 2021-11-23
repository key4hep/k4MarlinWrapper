import ROOT
import sys

f = ROOT.TFile.Open(sys.argv[1], "READ")
tree = f.Get("events")
print(tree.GetEntries())
