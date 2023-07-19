import { render, createVNode } from "vue";
import ReportDialog from "./ReportDialog.vue";

export default function (options = { reporttime: String, deviceid: String, reportdata: String }) {
    const mountNode = document.createElement("div")
    const app = createVNode(ReportDialog, options)
    render(app, mountNode)
    document.body.appendChild(mountNode)
}