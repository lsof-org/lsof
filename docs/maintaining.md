# Maintaining

## The basic rules

a Dialect maintainer can do as one's want as far as what one's will do
has no impact on the other dialects.

The following three dialects have ACTIVE maintainers:

FreeBSD

	@lrosenman
	@jiegec

Linux
	
	@dilinger
	@masatake
	@jiegec

NetBSD

	@tnn2
	@jiegec

Darwin

	@jiegec

OpenBSD

	@jiegec

Solaris/OpenIndiana

	@jiegec


@masatake is going to step down. See https://github.com/lsof-org/lsof/issues/192 .

If you find a good person who has interested in maintain a dialect
that none maintains, you can invite the person to the lsof-org
organization.

See also https://github.com/lsof-org/lsof/issues/157#issuecomment-941591778 .

## Git

A dialect maintainer must know Git.

### Sophisticated commits

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

### Commit logs

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

## Reviewing and merging

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

## Testing

Having a test case for a change is nice. The original lsof developed
and maintained by Vic Abell has a test mechanism. However, I started
developing my own test mechanism (the github test mechanism) written
in bash.

About the original test mechanism, see tests/00README.

About the the github test mechanism, see tests/case-00-hello.bash .
You can put a test case non-dialect specific, put tests/.
dialects/linux/tests/ is for testing linux specific code.

## Releasing

A dialect maintainer can release a new version of lsof FOR YOUR
DIALECT without getting permission from maintainers of the other
dialects.  So we can work asynchronously.

### Install ksh

The script for making an archive needs ksh.


### Implement "make dist" for your favorite dialect

See https://github.com/lsof-org/lsof/pull/131 how @masatake does for
linux dialect.

Merge the changes.


### Update the version and release date in the source tree if you need

You should update the version and release date in the following files:

- 00DIST
- version
- configure.ac

And then re-generated `docs/manpapage.md` by running `manpage.sh` in `docs` folder.

The version number has following form MAJOR.MINOR.MICRO.  When making a
release, update the version number.  If the change from the last release
is small, increment MICRO. If it is large, increment MINOR.

Merge the changes.

If you just want to make a release for a version already released in
another dialect, you don't need this step. Release the version number.

e.g.

conditions and situations:
@masatake already released 4.94.0 for linux.
No user visible change is committed to our git repo.
You may want to make the same release but for dialect freebsd.

In this case, you can use 4.94.0 as the version number.  If the
condition doesn't meet, use 4.94.1 (or 4.95.0).

### Put a tag with Git

NOTE: this step is an optional if the commit for releasing is already
tagged via another dialect.

The tag you will put should have the same name as the version given in
the step 2.

Do "git tag" like:

  $ git tag 4.95.0-this-is-an-example
  $ git push upstream 4.95.0-this-is-an-example

If you took a mistake, you can delete the tag like:

  $ git tag -d 4.95.0-this-is-an-example
  $ git push upstream :4.95.0-this-is-an-example

### Make the source archive for the release

   $ ./Configure [dialect]
   $ make dist

The archive is put at support/ directory.

Checking whether you can build a lsof executable from the archive is
good idea.

e.g.

   $ cp support/lsof_4.94.0.linux.tar.bz2 /tmp
   $ cd /tmp
   $ tar jxvf lsof_4.94.0.linux.tar.bz2
   $ cd lsof_4.94.0.linux
   $ ./Configure linux
   $ make
   $ make check (if your dialect support the target)

### Visit https://github.com/lsof-org/lsof/releases

1. Click [Draft new release]
2. Choose a tag. Choose with the tag given in the step 3.
3. Fill "Release title". Use the following form lsof-${theVersionNumber}-${dialect}. e.g. lsof-4.94.0-linux
4. Fill "Describe this release". Copy and paste the changes described in 00DIST since the last release in your dialect.
5. Record the release date. Put the date of the release at the end of "Contents" of 00DIST.
	"Contents" is at the beginning of 00DIST.
6. Put the source archive generated step in 4. Click "Attach binaries by dropping them here or selecting them. ", then specify the archive file.
7. Check "This is a pre-release"
8. Click [Publish release]
9. Verify the release

After step 8, the browser may show the page for the release. You can
re-read the description and the source code archives. You can verify
what you did here.

If you find a fault or something, click the [Edit] in the page. You can
update the pre-release.

If you convince it is ready, click the [Edit] in the page, remove the
check at "This is a pre-release", then [Publish release]. The new
version is available at https://github.com/lsof-org/lsof/releases.
