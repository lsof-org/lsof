<!-- Use "lsof-legacy" in cirrus and coveralls because of historical reason  -->
[![Cirrus CI FreeBSD 11 Build Status](https://api.cirrus-ci.com/github/lsof-org/lsof-legacy.svg?task=freebsd11&branch=master)](https://cirrus-ci.com/github/lsof-org/lsof-legacy)
[![Cirrus CI FreeBSD 12 Build Status](https://api.cirrus-ci.com/github/lsof-org/lsof-legacy.svg?task=freebsd12&branch=master)](https://cirrus-ci.com/github/lsof-org/lsof-legacy)
[![Travis CI Linux Build Status](https://travis-ci.org/lsof-org/lsof.svg?branch=master)](https://travis-ci.org/lsof-org/lsof)
[![Coveralls Linux Coverage Status on Travis CI](https://coveralls.io/repos/github/lsof-org/lsof-legacy/badge.svg?branch=master)](https://coveralls.io/github/lsof-org/lsof-legacy?branch=master)

# lsof
lsof-org at GitHub team takes over the maintainership of lsof
originally maintained by Vic Abell. This repository is for maintaining
the final source tree of lsof inherited from Vic. "legacy" branch
keeps the original source tree. We will not introduce any changes to
the "legacy" branch. This branch is just for reference.

"master" branch is used for maintenance. Bug fixes and enhancements go
to "master" branch.

lsof has supported many OSes. A term "dialect" represents code for
supporting a OSes. Because of limitted resources, we will maintain the
part of them. The current status of maintaince is as follows:

<dl>
<dt>freebsd</dt>
<dd>partially maintained, and tested on Cirrus CI</dd>
<dt>linux</dt>
<dd>fully maintained, and tested on Travis CI</dd>
<dt>darwin</dt>
<dd>not maintained, but tested on Travis CI</dd>
</dl>

If you are interested in maintaining a dialect, let us know via the
issue tracker of GitHub (https://github.com/lsof-org/lsof).  If
we cannot find a volunteer for a dialect, we will remove the dialect.

Many texts in the source tree still refers purdue.edu as the home of
lsof development. It should be https://github.com/lsof-org/lsof, the
new home. The updating is in progress.

We are running another repository, lsof-org/"lsof-linux" derived from
lsof-4.91 that is also released by Vic. To the repository, we have
introduced some new features and fixes for building specific to
GNU/Linux. We will merge the fruits of "lsof-linux" repository to this
repository incrementally. If you are using GNU/Linux, you may want to
use the code in "lsof-linux" repository.

lsof-org at GitHub team
