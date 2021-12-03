# CS2106 Labs
Lab Group: B09

- Notes on C (TODO: put link here)

## SoC Compute-Cluster OS
>Ubuntu 20.04 (64-bit)
Important point about 64-bit because it affects some of your variables (e.g `long`)

## Always use valgrind to check for memory leaks
You WILL be penalized for memory leaks.
```bash
valgrind ./ex2 < sample.in > sample.result.out
```

## Testing and Submitting your Assignment
### Testing
You may use xcne0 - xcne7 to test your assignments before submissions.

### Submission
Your submissions will be tested on the following nodes on the SoC Compute Cluster:
- xcne5
- xcne6
- xcne7
These nodes have been reseerved for CS2106 for the entire semester.

# Instructions for Labs
## Developing and Testing Code
### On MacBook Pro (Develope code)
1. Code on MacBook Pro using Sublime
2. Push code to GitHub repository

### On Ubuntu VM (Test code)
1. Pull code from GitHub repository
2. Run code

## Submitting Code (using SFTP)
Once done, submit code to SoC Compute Cluster node (xcne5-7) from MacBook Pro using the following steps:

### [STEP 1] Launch SFTP
Using xcne6, 
```bash
sftp -J jamesyys@sunfire.comp.nus.edu.sg jamesyys@xcne6
```

### [STEP 2] Upload file to SoC Compute Cluster node
```bash
# transfer file from local to remote (UPLOAD)
put localFilename
```

<details>
<summary>Additional notes about SFTP</summary>

### Running terminal commands on `local` machine (prepend 'l' to every command)
```bash
# EXAMPLES

# list files
lls 
# print address of current directory
lpwd
```
### Running terminal commands on `remote` machine (no change - use as per normal)
```bash
# EXAMPLES

# list file
ls
# print address of current directory
pwd
```

### Transferring Files
```bash
# from local to remote (UPLOAD) <--- this is the main one you will use
put localFilename # for a single file
put -r localFolder # for a folder 

# from remote to local (DOWNLOAD)
get remoteFilename
```
</details>

<details>
<summary>Alternative Instructions: Coding directly on the remote nodes (xcne0-7) </summary>
## Alternative Method

### Connecting to a Node (2 steps)
#### [Step 1] Log into Sunfire first
```bash
ssh jamesyys@sunfire.comp.nus.edu.sg
```
Please do not develop on the sunfire remote node, as it is not running the same OS as the xcne0-7 nodes.

#### [Step 2] Log into a node
```bash
ssh xcne6
```
##### Performing both steps together
Use the `-J` flag to chain ssh commands
```bash
ssh -J jamesyys@sunfire.comp.nus.edu.sg jamesyys@xcne6
```

### Get Lab Files
#### Download Lab Files
```bash
wget https://www.comp.nus.edu.sg/~ccris/cs2106_ay2122s1/lab1.tar.gz
```

#### Setup Lab Files
To setup the lab files, which will be given in an archive "labX.tar.gz", run the following command in terminal:
```bash
tar -xf lab1.tar.gz
```
</details>


# Additional Notes
## Software Configuration for SoC Compute Cluster nodes:
> Ubuntu 20.04
> gcc version 9.3.0
> GNU bash, version 5.0.17(1)-release


