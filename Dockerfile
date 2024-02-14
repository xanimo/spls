ARG PLATFORM
FROM $PLATFORM/debian:bullseye-slim AS build

COPY ./ .

RUN apt-get update && apt-get install --no-install-recommends -y \
    gosu \
    && rm -rf /var/lib/apt/lists/*

ENTRYPOINT [ "./scripts/entrypoint" ]

CMD [ "./bin/spvnode" ]
