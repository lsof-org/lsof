========================================================================
The handbook for lsof maintainers
========================================================================

Feel free to update this documentation.


The basic rules
========================================================================
a Dialect maintainer can do as one's want as far as what one's will do
has no impact on the other dialects.

The following three dialects have ACTIVE maintainers:

FreeBSD

	@lrosenman

Linux
	
	@dilinger
	@masatake
	@jiegec

NetBSD

	@tnn2

Darwin

	@jiegec


I, @masatake, is going to step down. See https://github.com/lsof-org/lsof/issues/192 .

If you find a good person who has interested in maintain a dialect
that none maintains, you can invite the person to the lsof-org
organization.

See also https://github.com/lsof-org/lsof/issues/157#issuecomment-941591778 .

Git
========================================================================

A dialect maintainer must know Git.

Sophisticated commits
========================================================================

You will change the repository. You will write the code or you will merge
the code submitted by a contributor via pull request.

About your dialect, you can do as you want. However, I recommend you to
accept only SOPHISTICATED commits only. About a change for the common code
between dialect, we accept only sophisticated commits. About a change having
impact on the other dialects, we accept only sophisticated commits.

One sophisticated commit does one thing.
e.g. a commit including a change for adjusting white-spaces and a change for
adding an option is not a sophisticated commit.

* https://github.com/lsof-org/lsof/pull/195#discussion_r825328892
* https://github.com/lsof-org/lsof/pull/208

Sophisticated commits help reviewers.
Understanding sophisticated commits is easy.

https://github.com/lsof-org/lsof/pull/213
This is not a sophisticated commit. Improving the way to hash and fixing
a typo are done in a commit. I didn't have energy to ask the contributor
to split the commit into two.

"Try-and-error" commits should not be merged. It makes the git log of
lsof dirty. Such dirtiness make us hard to read and understand git-log
and to do git-bisect effectively. You can ask the contributor to
squash the commits into one as far as the commits are for doing one
thing; squashed commit should be sophisticated.

Commit logs
========================================================================

If a change is dialect specific, use `[dialect]` as the prefix of the
header of the commit log like::

   [linux] compile with -Wall option
   [linux] delete unused variables
   [freebsd] cirrus-ci: disabled

We should the force this rule to us and contributors. So we can know
who should review the change/pull request. If no `[dialect]` prefix is
given to a commit, the change may have an impact across dialects; all
we must review the commit.

I have seen contributors write much expiation about their change
at the first comment of their pull request. However, their commit
log is very short. We should ask them to copy the first comment to
the commit log.

Reviewing and merging
========================================================================

A dialect maintainer has a responsibility to review pull requests that
change the code of one's dialect.

After merging you may have to update two documents:

00DIST

  If a pull request you merged changes user visible aspect of lsof,
  put the change at the end of 00DIST file. You can ask the contributor
  making a pull request to put an entry for the one's change instead.

00CREDITS

  If this is the first time for the contributor to contribute to lsof,
  add one's name to 00CREDITS.

You may have to the contributor to update the lsof.8 man page.
If a pull request fixes the issue already reported, put
`Close #issueNumber` to the first comment of the pull request.

e.g. https://github.com/lsof-org/lsof/pull/199#issue-1193523123


Testing
========================================================================

Having a test case for a change is nice. The original lsof developed
and maintained by Vic Abell has a test mechanism. However, I started
developing my own test mechanism (the github test mechanism) written
in bash.

About the original test mechanism, see tests/00README.

About the the github test mechanism, see tests/case-00-hello.bash .
You can put a test case non-dialect specific, put tests/.
dialects/linux/tests/ is for testing linux specific code.

Releasing
========================================================================

See HOW_TO_RELEASE.rst.
