# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure('2') do |config|
  config.vm.box = 'generic/debian9'
  config.vm.box_check_update = false

  config.vm.hostname = 'parameters'

  config.vm.synced_folder '.', '/app'

  config.vm.provider 'virtualbox' do |vb|
    vb.gui = false
    vb.memory = 1024
  end

  config.vm.provision 'shell' do |s|
    s.env = {
      'DEBIAN_FRONTEND' => 'noninteractive',
      'WORKING_DIR' => '/app',
      'BASHRC_FILE' => '/home/vagrant/.bashrc',
      'FOX21AT_SOURCES_LIST' => '/etc/apt/sources.list.d/fox21at.list',
    }
    s.inline = <<-SHELL
      echo "cd ${WORKING_DIR}" >> ${BASHRC_FILE}
      echo "alias ll='ls -la'" >> ${BASHRC_FILE}
      echo "alias l='ls -l'" >> ${BASHRC_FILE}

      apt-get update -yqq
      apt-get upgrade -y

      wget -qO - https://fox21.at/christian_mayer.asc | sudo apt-key add -
      echo "deb http://debian.fox21.at/ stretch main" >> ${FOX21AT_SOURCES_LIST}

      apt-get update -yqq
      #apt-get install -y cmake clang-7 libboost-filesystem1.67-dev

      echo 'done'
    SHELL
  end
end
