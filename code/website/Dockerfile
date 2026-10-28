FROM nginxinc/nginx-unprivileged:1.27-alpine

COPY nginx.conf /etc/nginx/conf.d/default.conf
COPY security-headers.inc /etc/nginx/conf.d/security-headers.inc
COPY --chown=nginx:nginx . /usr/share/nginx/html
# The two config files above must be in the build context, so they were copied
# into the web root too. Remove them so they are not served to visitors.
USER root
RUN rm -f /usr/share/nginx/html/nginx.conf /usr/share/nginx/html/security-headers.inc
USER nginx

EXPOSE 8080
HEALTHCHECK --interval=30s --timeout=3s --start-period=5s \
  CMD wget -qO- http://127.0.0.1:8080/healthz >/dev/null || exit 1