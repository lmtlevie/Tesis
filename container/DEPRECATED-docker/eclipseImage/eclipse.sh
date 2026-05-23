#!/bin/bash

# Preferences are in the correct place (can not copy them during the image creation as it depends on the current user). TODO: after docker 1.9 we could using the ARGS)
if [ ! -d "$HOME/.eclipse/org.eclipse.platform_4.7.3_1473617060_linux_gtk_x86_64/configuration/.settings/org.eclipse.ui.ide.prefs" ]; then
  mkdir -p $HOME/.eclipse/org.eclipse.platform_4.7.3_1473617060_linux_gtk_x86_64/configuration/.settings/
  cp /opt/eclipse/configuration/.settings/org.eclipse.ui.ide.prefs $HOME/.eclipse/org.eclipse.platform_4.7.3_1473617060_linux_gtk_x86_64/configuration/.settings/org.eclipse.ui.ide.prefs
fi
exec /opt/eclipse/eclipse
