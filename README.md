# Violet

<p align='center'><img src='logo.webp' width=30%></p>

Violet is a simple static website generator written in C. Violet allows users
to write pages in mark down and will use a configuration file to build a website
how users would like. Though there will be an example site where
css will be included, you will need to write own css for the customization of the
website.

## Quick Start

After cloning the violet directory, the follow commands can be run depending on
your OS to get a quick start with violet. GCC complier must be installed on the
system for the build scripts to work.

**Linux:**
```
$ chmod +x ./build.sh
$ ./build.sh
$ ./bin/violet
```

**Windows:**
```
$ .\build.bat
$ .\bin\violet
```

## Supported Markdown

**Heading**
```
# H1
## H2
### H3
```

**Bold**
```
**Bold Text**
```

**Italic**
```
*Italicized Text*
```

**Blockquote**
```
> Blockquote
```

**Code**
```
`Code`
```

### Needs To Be Implemented

**Ordered List**
```
1. First Item
2. Second Item
3. Third Item
```

**Unordered List**
```
- First Item
- Second Item
- Third Item
```

**Break**
```
---
```

**Link**
```
[Title](https://www.website.com)
```

**Image**
```
![Alt Text](image.jpg)
```

## To Contribute

There are no major rules in terms of pull requests at the moments for requriements,
other than the changes must be on a seperate branch, where the name of the branch
clear outlines the feature that is being changed. I would also advise using the
same general code format that is currently being used.

