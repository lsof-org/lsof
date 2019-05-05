# lsof-legacy
lsof-org team (or lsof at GitHub team) takes over the maintainership
of lsof originally maintained by Vic Abell. This repository is for
maintaining the final source tree of lsof received from Vic.

This repository has two branches "master" and "cleanup".

"master" branch keeps the original source tree. We will not introduce
any changes to the "master" branch. This branch is just for reference.

The "master" branch includes directories and files that we don't need
to maintain the source tree at github.com. We would like to remove
such directories and files from the source tree before adding new
features. We will commit the changes removing them to "cleanup" branch
incrementally.

After cleaning up is done, we will make a new repository called "lsof5"
based on the "cleanup" branch of "lsof-legacy" repository. The "lsof5"
repository will be the repository you may want to add new features.

lsof has supported many OSes. A term "dialect" represents code for
supporting a OSes. Because of limitted resources we will maintain the
part of them: GNU/Linux and FreeBSD. We will remove dialects for the
rest of them. If you are interested in maintaining a dialect, let us
know via the issue tracker of GitHub.

We are running another repository, lsof-org/"lsof-linux" derived from
lsof-4.91 that is also released by Vic. To the repository, we have
introduced some new features and fixes for building specific to
GNU/Linux. We will merge the fruits of "lsof-linux" repository to
"lsof5" repository in the future. If you are using GNU/Linux, you may
want to use the code in "lsof-linux" repository.

lsof at GitHub team
