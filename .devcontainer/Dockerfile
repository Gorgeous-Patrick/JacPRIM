# Use the prebuilt image from johnramsden/upmem
FROM johnramsden/upmem:latest

# Set default working directory
WORKDIR /workspace

# Optional: install cmake and ninja if not already installed
RUN apt-get update && apt-get install -y \
    cmake \
    ninja-build \
    && apt-get clean

# Set default user if needed
USER upmem
