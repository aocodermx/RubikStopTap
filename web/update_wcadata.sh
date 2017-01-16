
# Check for changes in worldcubeassociation site.
# if changes
#   download sql file
#   select data
#   update table

# when tested, add cron job
mysql site_default -u site_default -p < WCA_export.sql
