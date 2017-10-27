

TFile *open_dqm_archive_root_macro(const std::string &archive)
{
  gSystem->Load("/opt/dqmsoftware/dqm4hep/lib/libDQMCore.so");
  TFile * _file0 = TFile::Open(archive.c_str(), archive.c_str());

  return _file0;
}
