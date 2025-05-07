FROM texlive/texlive AS development

RUN apt update
RUN apt install inkscape -y

WORKDIR /docs

COPY docs /docs

RUN chmod +x /docs/entrypoint.sh

ENTRYPOINT ["sh", "entrypoint.sh"]