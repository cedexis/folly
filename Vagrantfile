# -*- mode: ruby -*-
# vi: set ft=ruby :

# Vagrantfile API/syntax version. Don't touch unless you know what you're doing!
VAGRANTFILE_API_VERSION = "2"

def provision_instance(config)
  config.vm.provision "shell", inline: <<-SHELL
    rm -f /etc/network/interfaces.d/eth1.cfg
    echo "auto eth0" >> /etc/network/interfaces.d/eth0.cfg
    echo "iface eth0 inet static" >> /etc/network/interfaces.d/eth0.cfg
    echo "address 192.168.35.25" >> /etc/network/interfaces.d/eth0.cfg
    echo "netmask 255.255.255.0" >> /etc/network/interfaces.d/eth0.cfg
    ifdown eth1 && ifup eth1
    sudo apt-get update
    sudo apt-get -y install apt-transport-https ca-certificates curl software-properties-common
    curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -
    sudo add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable"
    sudo apt-get update
    sudo apt-get -y install docker-ce
    sudo usermod -a -G docker vagrant
  SHELL
end

Vagrant.configure(VAGRANTFILE_API_VERSION) do |config|
  # Vagrant base image defaults to trusty64
  config.vm.box = "minimal/trusty64"

  # Share a folder to the guest VM.
  config.vm.synced_folder ".", "/vagrant"

  config.vm.provider :virtualbox do |vb|
    vb.customize ["modifyvm", :id, "--memory", 6144]
    vb.customize ["modifyvm", :id, "--cpus", 2]
    vb.customize ["modifyvm", :id, "--ioapic", "on"]

    ### Change network card to PCnet-FAST III
    # For NAT adapter
    vb.customize ["modifyvm", :id, "--nictype1", "Am79C973"]
    # For host-only adapter
    vb.customize ["modifyvm", :id, "--nictype2", "Am79C973"]
  end

  config.vm.define :xenial64, autostart: false do | xenial64 |
    xenial64.vm.box = "minimal/xenial64"
    xenial64.vm.network "private_network", ip: "192.168.50.5", auto_config: false
    provision_instance(config)
  end

end
