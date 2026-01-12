#!/bin/bash

NODES=${1:-4}

echo "Requesting $NODES nodes interactively..."

srun \
  --nodes=$NODES \
  --ntasks-per-node=1 \
  --time=00:20:00 \
  --pty bash
