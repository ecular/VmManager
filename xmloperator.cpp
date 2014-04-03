#include "xmloperator.h"
#include <QString>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomAttr>
#include <QFile>
#include <QTextStream>
#include <vector>



XMLOperator::XMLOperator()
{

}

int XMLOperator::AddNode(QString file_location, QString vm_name, QString vcpu_arg, QString memory_arg, QString privateLevel_arg, QString imgPath, QString rootimgPath, QString macAddress, QString xmlLocation)
{
    QDomDocument doc;
    QFile file(file_location);
    if(!file.open(QIODevice::ReadOnly))
        return 1;
    QDomElement root;
    if(!doc.setContent(&file))
    {
        QDomProcessingInstruction instruction;
        instruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding = \"UTF-8\"");
        doc.appendChild(instruction);
        root = doc.createElement(QString("VMs"));
        doc.appendChild(root);
    }
    else
        root = doc.documentElement();
    file.close();

    /*vm name*/
    QDomElement note = doc.createElement("VM");
    QDomAttr id = doc.createAttribute("name");
    id.setValue(vm_name);
    note.setAttributeNode(id);
    root.appendChild(note);

    /*vcpu*/
    QDomElement vcpu = doc.createElement("vcpu");
    note.appendChild(vcpu);
    QDomText vcpu_text = doc.createTextNode(vcpu_arg);
    vcpu.appendChild(vcpu_text);

    /*memory*/
    QDomElement memory = doc.createElement("memory");
    note.appendChild(memory);
    QDomText memory_text = doc.createTextNode(memory_arg);
    memory.appendChild(memory_text);

    /*PrivateLevel*/
    QDomElement privateLevel = doc.createElement("PrivateLevel");
    note.appendChild(privateLevel);
    QDomText privateLevel_text = doc.createTextNode(privateLevel_arg);
    privateLevel.appendChild(privateLevel_text);

    /*PrivateImgPath*/
    QDomElement ImgPath = doc.createElement("PrivateImgPath");
    note.appendChild(ImgPath);
    QDomText ImgPath_text = doc.createTextNode(imgPath);
    ImgPath.appendChild(ImgPath_text);

    /*xmlLocation*/
    QDomElement XMLLocation = doc.createElement("XmlLocation");
    note.appendChild(XMLLocation);
    QDomText XMLLocation_text = doc.createTextNode(xmlLocation);
    XMLLocation.appendChild(XMLLocation_text);

    /*RootImgPath*/
    QDomElement RootImgPath = doc.createElement("RootImgPath");
    note.appendChild(RootImgPath);
    QDomText RootImgPath_text = doc.createTextNode(rootimgPath);
    RootImgPath.appendChild(RootImgPath_text);

    /*Mac*/
    if(macAddress != "")
    {
        QDomElement Mac = doc.createElement("Mac");
        note.appendChild(Mac);
        QDomText Mac_text = doc.createTextNode(macAddress);
        Mac.appendChild(Mac_text);
    }

    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return 1;
    QTextStream out(&file);
    out.setCodec("UTF-8");
    doc.save(out, 4, QDomNode::EncodingFromTextStream);
    file.close();
    return 0;
}

int XMLOperator::DelNode(QString vm_name)
{
    QDomDocument doc;
    QFile file("./test.xml");
    if(!file.open(QIODevice::ReadOnly))
        return 1;

    if(!doc.setContent(&file))
    {
        file.close();
        return 1;
    }
    file.close();

    /*root element*/
    QDomElement root = doc.documentElement();
    QDomNode VM = root.firstChild();

    while(!VM.isNull())
    {
        if(VM.isElement())
        {
            QDomElement e = VM.toElement();
            if(e.tagName() == "VM" && e.attribute("name") == vm_name)
            {
                root.removeChild(VM);
                if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
                    return 1;
                QTextStream out(&file);
                out.setCodec("UTF-8");
                doc.save(out, 4, QDomNode::EncodingFromTextStream);
                file.close();
                return 0;
            }
        }
        VM = VM.nextSibling();
    }
    return 1;
}

QString XMLOperator::GetElement(QString vm_name, QString element)
{
    QDomDocument doc;
    QFile file("./test.xml");
    if(!file.open(QIODevice::ReadOnly))
        return NULL;

    if(!doc.setContent(&file))
    {
        file.close();
        return NULL;
    }
    file.close();

    /*root element*/
    QDomElement root = doc.documentElement();
    QDomNode VM = root.firstChild();

    while(!VM.isNull())
    {
        if(VM.isElement())
        {
            QDomElement e = VM.toElement();
            if(e.tagName() == "VM" && e.attribute("name") == vm_name)
            {
                QDomNodeList list = e.childNodes();
                for(int i = 0; i < list.count(); ++i)
                {
                    QDomNode node = list.at(i);
                    if(node.toElement().tagName() == element)
                        return node.toElement().text();
                }
            }
        }
        VM = VM.nextSibling();
    }
    return NULL;
}

int XMLOperator::CreateVMXml(QString file_location, QString vm_name, QString vcpu, QString memory, QString PrivateimgPath, QString RootimgPath, QString macAddress)
{
    QDomDocument doc;

    /*root*/
    QDomProcessingInstruction instruction;
    instruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding = \"UTF-8\"");
    doc.appendChild(instruction);
    QDomElement root = doc.createElement(QString("domain"));
    doc.appendChild(root);
    QDomAttr type = doc.createAttribute("type");
    type.setValue("kvm");
    root.setAttributeNode(type);
    QDomAttr qemu = doc.createAttribute("xmlns:qemu");
    qemu.setValue("http://libvirt.org/schemas/domain/qemu/1.0");
    root.setAttributeNode(qemu);

    /*name*/
    QDomElement name_node = doc.createElement("name");
    QDomText name_text = doc.createTextNode(vm_name);
    name_node.appendChild(name_text);
    root.appendChild(name_node);

    /*memory*/
    QDomElement memory_node = doc.createElement("memory");
    QDomText memory_text = doc.createTextNode(memory);
    QDomAttr unit = doc.createAttribute("unit");
    unit.setValue("KiB");
    memory_node.setAttributeNode(unit);
    memory_node.appendChild(memory_text);
    root.appendChild(memory_node);

    /*current memory*/
    QDomElement currentmemory_node = doc.createElement("currentMemory");
    QDomText currentmemory_text = doc.createTextNode(memory);
    QDomAttr currentunit = doc.createAttribute("unit");
    currentunit.setValue("KiB");
    currentmemory_node.setAttributeNode(currentunit);
    currentmemory_node.appendChild(currentmemory_text);
    root.appendChild(currentmemory_node);

    /*vcpu*/
    QDomElement vcpu_node = doc.createElement("vcpu");
    QDomText vcpu_text = doc.createTextNode(vcpu);
    vcpu_node.appendChild(vcpu_text);
    root.appendChild(vcpu_node);

    /*os*/
    QDomElement os_node = doc.createElement("os");
    QDomElement type_node = doc.createElement("type");
    QDomAttr arch = doc.createAttribute("arch");
    arch.setValue("x86_64");
    QDomAttr machine = doc.createAttribute("machine");
    machine.setValue("pc");
    type_node.setAttributeNode(arch);
    type_node.setAttributeNode(machine);
    QDomText type_text = doc.createTextNode("hvm");
    type_node.appendChild(type_text);
    os_node.appendChild(type_node);
    QDomElement boot_node = doc.createElement("boot");
    QDomAttr dev = doc.createAttribute("dev");
    dev.setValue("hd");
    boot_node.setAttributeNode(dev);
    os_node.appendChild(boot_node);
    root.appendChild(os_node);

    /*features*/
    QDomElement feature_node = doc.createElement("features");
    QDomElement acpi_node = doc.createElement("acpi");
    QDomElement apic_node = doc.createElement("apic");
    QDomElement pae_node = doc.createElement("pae");
    feature_node.appendChild(acpi_node);
    feature_node.appendChild(apic_node);
    feature_node.appendChild(pae_node);
    root.appendChild(feature_node);

    /*clock*/
    QDomElement clock_node = doc.createElement("clock");
    QDomAttr offset = doc.createAttribute("offset");
    offset.setValue("localtime");
    clock_node.setAttributeNode(offset);
    root.appendChild(clock_node);

    /*on_poweroff*/
    QDomElement on_poweroff_node = doc.createElement("on_poweroff");
    QDomText on_poweroff_text = doc.createTextNode("destroy");
    on_poweroff_node.appendChild(on_poweroff_text);
    root.appendChild(on_poweroff_node);

    /*on_reboot*/
    QDomElement on_reboot_node = doc.createElement("on_reboot");
    QDomText on_reboot_text = doc.createTextNode("restart");
    on_reboot_node.appendChild(on_reboot_text);
    root.appendChild(on_reboot_node);

    /*on_crash*/
    QDomElement on_crash_node = doc.createElement("on_crash");
    QDomText on_crash_text = doc.createTextNode("restart");
    on_crash_node.appendChild(on_crash_text);
    root.appendChild(on_crash_node);

    /*device*/
    QDomElement devices_node = doc.createElement("devices");

    QDomElement emulator_node = doc.createElement("emulator");
    QDomText emulator_text = doc.createTextNode("/usr/local/bin/qemu-system-x86_64");
    emulator_node.appendChild(emulator_text);
    devices_node.appendChild(emulator_node);

    /*disk1*/
    QDomElement disk_node = doc.createElement("disk");
    QDomAttr disk_type = doc.createAttribute("type");
    disk_type.setValue("file");
    QDomAttr disk_device = doc.createAttribute("device");
    disk_device.setValue("disk");
    disk_node.setAttributeNode(disk_type);
    disk_node.setAttributeNode(disk_device);

    QDomElement driver_node = doc.createElement("driver");
    QDomAttr disk_name = doc.createAttribute("name");
    disk_name.setValue("qemu");
    QDomAttr driver_type = doc.createAttribute("type");
    driver_type.setValue("qcow2");
    driver_node.setAttributeNode(disk_name);
    driver_node.setAttributeNode(driver_type);
    disk_node.appendChild(driver_node);

    QDomElement source_node = doc.createElement("source");
    QDomAttr source_file = doc.createAttribute("file");
    source_file.setValue(RootimgPath);
    source_node.setAttributeNode(source_file);
    disk_node.appendChild(source_node);

    QDomElement target_node = doc.createElement("target");
    QDomAttr target_dev = doc.createAttribute("dev");
    target_dev.setValue("hda");
    QDomAttr target_bus = doc.createAttribute("bus");
    target_bus.setValue("virtio");
    target_node.setAttributeNode(target_dev);
    target_node.setAttributeNode(target_bus);
    disk_node.appendChild(target_node);

    devices_node.appendChild(disk_node);

    /*disk2*/
    QDomElement disk1_node = doc.createElement("disk");
    QDomAttr disk1_type = doc.createAttribute("type");
    disk1_type.setValue("block");
    QDomAttr disk1_device = doc.createAttribute("device");
    disk1_device.setValue("disk");
    disk1_node.setAttributeNode(disk1_type);
    disk1_node.setAttributeNode(disk1_device);

    QDomElement driver1_node = doc.createElement("driver");
    QDomAttr disk1_name = doc.createAttribute("name");
    disk1_name.setValue("qemu");
    QDomAttr driver1_type = doc.createAttribute("type");
    driver1_type.setValue("qcow2");
    driver1_node.setAttributeNode(disk1_name);
    driver1_node.setAttributeNode(driver1_type);
    disk1_node.appendChild(driver1_node);

    QDomElement source1_node = doc.createElement("source");
    QDomAttr source1_file = doc.createAttribute("dev");
    source1_file.setValue(PrivateimgPath);
    source1_node.setAttributeNode(source1_file);
    disk1_node.appendChild(source1_node);

    QDomElement target1_node = doc.createElement("target");
    QDomAttr target1_dev = doc.createAttribute("dev");
    target1_dev.setValue("hdb");
    QDomAttr target1_bus = doc.createAttribute("bus");
    target1_bus.setValue("virtio");
    target1_node.setAttributeNode(target1_dev);
    target1_node.setAttributeNode(target1_bus);
    disk1_node.appendChild(target1_node);

    devices_node.appendChild(disk1_node);

    /*video*/
    QDomElement video_node = doc.createElement("video");
    QDomElement model_node = doc.createElement("model");
    QDomAttr model_type = doc.createAttribute("type");
    model_type.setValue("vmvga");
    model_node.setAttributeNode(model_type);
    video_node.appendChild(model_node);
    devices_node.appendChild(video_node);

    /*input*/
    QDomElement input_node = doc.createElement("input");
    QDomAttr input_type = doc.createAttribute("type");
    input_type.setValue("mouse");
    QDomAttr input_bus = doc.createAttribute("bus");
    input_bus.setValue("ps2");
    input_node.setAttributeNode(input_type);
    input_node.setAttributeNode(input_bus);
    devices_node.appendChild(input_node);

    /*graphics*/
    QDomElement graphics_node = doc.createElement("graphics");
    QDomAttr graphics_type = doc.createAttribute("type");
    graphics_type.setValue("vnc");
    QDomAttr graphics_port = doc.createAttribute("port");
    graphics_port.setValue("-1");
    QDomAttr graphics_autoport = doc.createAttribute("autoport");
    graphics_autoport.setValue("yes");
    QDomAttr graphics_listen = doc.createAttribute("listen");
    graphics_listen.setValue("0.0.0.0");
    QDomAttr graphics_keymap = doc.createAttribute("keymap");
    graphics_keymap.setValue("en-us");
    graphics_node.setAttributeNode(graphics_type);
    graphics_node.setAttributeNode(graphics_port);
    graphics_node.setAttributeNode(graphics_autoport);
    graphics_node.setAttributeNode(graphics_listen);
    graphics_node.setAttributeNode(graphics_keymap);
    devices_node.appendChild(graphics_node);

    /*interface*/
    if(macAddress != "")
    {
        QDomElement interface_node = doc.createElement("interface");
        QDomAttr interface_type = doc.createAttribute("type");
        interface_type.setValue("vde");
        interface_node.setAttributeNode(interface_type);

        QDomElement switch_node = doc.createElement("switch");
        QDomAttr switch_path = doc.createAttribute("path");
        switch_path.setValue("/tmp/vde.ctl");
        switch_node.setAttributeNode(switch_path);
        interface_node.appendChild(switch_node);

        QDomElement mac_node = doc.createElement("mac");
        QDomAttr mac_address = doc.createAttribute("address");
        mac_address.setValue(macAddress);
        mac_node.setAttributeNode(mac_address);
        interface_node.appendChild(mac_node);

        QDomElement interface_model_node = doc.createElement("model");
        QDomAttr interface_model_type = doc.createAttribute("type");
        interface_model_type.setValue("virtio");
        interface_model_node.setAttributeNode(interface_model_type);
        interface_node.appendChild(interface_model_node);

        devices_node.appendChild(interface_node);
    }


    root.appendChild(devices_node);

    /*commandline 1 */
    QDomElement commandline_node = doc.createElement("qemu:commandline");
    QDomElement commandline_arg_node = doc.createElement("qemu:arg");
    QDomAttr commandline_arg_value = doc.createAttribute("value");
    commandline_arg_value.setValue("--" + RootimgPath);
    commandline_arg_node.setAttributeNode(commandline_arg_value);
    commandline_node.appendChild(commandline_arg_node);
    root.appendChild(commandline_node);

    /*commandline 2 */
    QDomElement commandline2_node = doc.createElement("qemu:commandline");
    QDomElement commandline2_arg_node = doc.createElement("qemu:arg");
    QDomAttr commandline2_arg_value = doc.createAttribute("value");
    commandline2_arg_value.setValue("--" + PrivateimgPath);
    commandline2_arg_node.setAttributeNode(commandline2_arg_value);
    commandline2_node.appendChild(commandline2_arg_node);
    root.appendChild(commandline2_node);

    QFile file(file_location);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return 1;

    QTextStream out(&file);
    out.setCodec("UTF-8");
    doc.save(out, 4, QDomNode::EncodingFromTextStream);
    file.close();
    return 0;
}

vector<QString> XMLOperator::GetVMnameVector()
{
    vector<QString> VMnameVector;
    QDomDocument doc;
    QFile file("./test.xml");
    if(!file.open(QIODevice::ReadOnly))
        return VMnameVector;

    if(!doc.setContent(&file))
    {
        file.close();
        return VMnameVector;
    }
    file.close();

    /*root element*/
    QDomElement root = doc.documentElement();
    QDomNode VM = root.firstChild();

    while(!VM.isNull())
    {
        if(VM.isElement())
        {
            QDomElement e = VM.toElement();
            if(e.tagName() == "VM")
            {
                VMnameVector.push_back(e.attribute("name"));
            }
        }
        VM = VM.nextSibling();
    }
    return VMnameVector;
}
