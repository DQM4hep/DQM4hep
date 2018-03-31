
# dqm4hep-tagger 

python script for automatic creation of (pre-)release tags of DQM4hep packages
Copied from [ILCInstall](https://github.com/iLCSoft/ILCInstall) package

Author: A. Sailer, CERN

* requires write privilegs on [https://github.com/DQM4HEP](https://github.com/DQM4HEP)


* usage:
```    
dqm4hep-tagger --packages dqm4hep-core dqm4hep-net dqm4hep-online ... [--properRelease] [--makeTags] [--file packageFile]
```

* default is dry run mode use --makeTags to actually make the tags

* default is pre-release tags, use --properRelease to make real release

* The script will parse github pull requests to collate release notes
  and write them into doc/ReleaseNotes.md

* The script will update the _VERSION_MAJOR, MINOR, PATH in the base
  CMakeLists.txt file to the new version

* One needs a github personal access token in a file called
  GitTokens.py in a constant called GITHUBTOKEN, see helperfunctions
  for details, also the ImportError will give this information

* Will not make another prerelease tag if there are no new commits
  since the last tag Release notes are always written for the new
  version

* to set the version number give the repo onwer (dqm4hep) and the
  version number afterwards dqm4hep-tagger --packages
  dqm4hep/dqm4hep-core/v02-03-07 ... [--properRelease] [--makeTags]

* can also use a configuration file which contains the same kind of
  strings

    dqm4hep-tagger --file configFile

* see dqm4hep-tagger --help as well
