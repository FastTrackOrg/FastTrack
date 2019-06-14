# Contributing

When contributing to this Fast Track, please first discuss the change you wish to make via email (benjamin.gallois@fasttrack.sh).

Please note we have a code of conduct, please follow it in all your interactions with the project.

## Installation

Before contributing to Fast Track, make sure to have a working environment of development, [see the developer guide](http://http://www.fasttrack.sh/API/index.html).
Fork the [project](http://https://git.benjamin-gallois.fr/bgallois/FastTrack).

## Contribute

## Create an account

To contribute to Fast Track, first, create an [account](https://git.benjamin-gallois.fr), and fork the Fast Track project.

## Branch

Before any work can be done, create a branch. If you working on a new feature, branch the dev branch. For a bug fixe, branch the master branch.

```
$ git checkout master
$ git pull master
$ git push origin master
$ git checkout -b feature/featureName
```

```
$ git checkout dev
$ git pull dev
$ git push origin dev
$ git checkout -b fix/fixName
```

## Create a merge request

Before creating the merge request, make sure that you follow the project guidelines. Pull request that will not follow all the guidelines will be automatically refused.
```
git push -u origin feature/featureName
```

```
$ git push -u origin -b fix/fixName
```

Go to the [project page](https://git.benjamin-gallois.fr/bgallois/FastTrack) and click on the **Merge Request** tab. Click on **New merge request**. Select the source branch and the target branch and click on **Compare branches and continue** button. Finally, click on the **Submit merge request**.

[More information](https://docs.gitlab.com/ee/gitlab-basics/add-merge-request.html).

## Guidelines

To contribute to Fast Track, you have to imperatively follow these guidelines.

## Test

Fast Track is using [Google Test framework](https://github.com/google/googletest).
* Each merge request has to pass all the tests, to run the test:
```
$ ./test.sh
```
* If you add some features to the tracking algorithm, you have to implement tests. New features to the tracking algorithm without tests will be refused.
	* New features that are purely on the graphical interface can omit tests.

## Formatting

Fast Track is using [Clang format](https://clang.llvm.org/docs/ClangFormat.html) to ensure the homogeneity of code formatting inside the project.
* Format your code according to the .clang-format file.
* Format your documentation following the [Doxygen](http://doxygen.nl/) formatting.
* Follow a standard C++ [naming convention](https://google.github.io/styleguide/cppguide.html#Naming).  


## Our Responsibilities

Project maintainers are responsible for clarifying the standards of acceptable behavior and are expected to take appropriate and fair corrective action in response to any instances of unacceptable behavior.

Project maintainers have the right and responsibility to remove, edit, or reject comments, commits, code, wiki edits, issues, and other contributions that are not aligned to this Code of Conduct, or to ban temporarily or permanently any contributor for other behaviors that they deem inappropriate, threatening, offensive, or harmful.